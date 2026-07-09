#include "feast/postprocessing/PostProcessor.hpp"

#include "feast/elements/Tet4.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace feast {
namespace {

constexpr double kPi = 3.141592653589793238462643383279502884;

Voigt6 vectorToVoigt(const Vector& v)
{
    if (v.size() != 6) {
        throw std::invalid_argument("Expected a 6-component vector.");
    }

    Voigt6 out{};
    for (std::size_t i = 0; i < 6; ++i) {
        out[i] = static_cast<double>(v[i]);
    }
    return out;
}

std::array<std::array<double, 3>, 3> voigtToMatrix(const Voigt6& stress)
{
    return {{
        {{stress[0], stress[3], stress[5]}},
        {{stress[3], stress[1], stress[4]}},
        {{stress[5], stress[4], stress[2]}}
    }};
}

double clampUnit(double value)
{
    if (value < -1.0) {
        return -1.0;
    }
    if (value > 1.0) {
        return 1.0;
    }
    return value;
}

} // namespace

ScalarSummary summarizeValues(const std::vector<double>& values)
{
    ScalarSummary summary;

    summary.count = values.size();
    if (values.empty()) {
        return summary;
    }

    double sum = 0.0;
    double minValue = values.front();
    double maxValue = values.front();

    for (double value : values) {
        sum += value;
        minValue = std::min(minValue, value);
        maxValue = std::max(maxValue, value);
    }

    summary.sum = sum;
    summary.mean = sum / static_cast<double>(values.size());
    summary.min = minValue;
    summary.max = maxValue;
    summary.range = maxValue - minValue;

    double varianceSum = 0.0;
    for (double value : values) {
        const double diff = value - summary.mean;
        varianceSum += diff * diff;
    }

    summary.standardDeviation =
        std::sqrt(varianceSum / static_cast<double>(values.size()));

    return summary;
}

ScalarSummary PostProcessor::summarize(const std::vector<double>& values)
{
    return summarizeValues(values);
}

double PostProcessor::magnitude(const Vector3& value)
{
    return std::sqrt(
        value[0] * value[0] +
        value[1] * value[1] +
        value[2] * value[2]);
}

std::vector<double> PostProcessor::magnitudes(const std::vector<Vector3>& values)
{
    std::vector<double> out;
    out.reserve(values.size());

    for (const auto& value : values) {
        out.push_back(magnitude(value));
    }

    return out;
}

double PostProcessor::hydrostaticStress(const Voigt6& stress)
{
    return (stress[0] + stress[1] + stress[2]) / 3.0;
}

std::vector<double> PostProcessor::hydrostaticStress(const std::vector<Voigt6>& stresses)
{
    std::vector<double> out;
    out.reserve(stresses.size());

    for (const auto& stress : stresses) {
        out.push_back(hydrostaticStress(stress));
    }

    return out;
}

double PostProcessor::deviatoricJ2(const Voigt6& stress)
{
    const double p = hydrostaticStress(stress);

    const double sxx = stress[0] - p;
    const double syy = stress[1] - p;
    const double szz = stress[2] - p;
    const double sxy = stress[3];
    const double syz = stress[4];
    const double szx = stress[5];

    return 0.5 * (
        sxx * sxx + syy * syy + szz * szz +
        2.0 * (sxy * sxy + syz * syz + szx * szx));
}

std::vector<double> PostProcessor::deviatoricJ2(const std::vector<Voigt6>& stresses)
{
    std::vector<double> out;
    out.reserve(stresses.size());

    for (const auto& stress : stresses) {
        out.push_back(deviatoricJ2(stress));
    }

    return out;
}

double PostProcessor::vonMises(const Voigt6& stress)
{
    return std::sqrt(3.0 * deviatoricJ2(stress));
}

std::vector<double> PostProcessor::vonMises(const std::vector<Voigt6>& stresses)
{
    std::vector<double> out;
    out.reserve(stresses.size());

    for (const auto& stress : stresses) {
        out.push_back(vonMises(stress));
    }

    return out;
}

Principal3 PostProcessor::principalStresses(const Voigt6& stress)
{
    const auto m = voigtToMatrix(stress);

    const double a = m[0][0];
    const double b = m[1][1];
    const double c = m[2][2];
    const double d = m[0][1];
    const double e = m[0][2];
    const double f = m[1][2];

    const double p1 = d * d + e * e + f * f;

    Principal3 out{};

    if (p1 < 1e-14) {
        out[0] = a;
        out[1] = b;
        out[2] = c;
        std::sort(out.begin(), out.end());
        return out;
    }

    const double q = (a + b + c) / 3.0;

    const double aa = a - q;
    const double bb = b - q;
    const double cc = c - q;

    const double p2 = aa * aa + bb * bb + cc * cc + 2.0 * p1;
    const double p = std::sqrt(p2 / 6.0);

    std::array<std::array<double, 3>, 3> B{};
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            B[i][j] = (m[i][j] - (i == j ? q : 0.0)) / p;
        }
    }

    const double detB =
        B[0][0] * (B[1][1] * B[2][2] - B[1][2] * B[2][1]) -
        B[0][1] * (B[1][0] * B[2][2] - B[1][2] * B[2][0]) +
        B[0][2] * (B[1][0] * B[2][1] - B[1][1] * B[2][0]);

    const double r = detB / 2.0;
    const double phi = std::acos(clampUnit(r)) / 3.0;

    out[0] = q + 2.0 * p * std::cos(phi);
    out[2] = q + 2.0 * p * std::cos(phi + (2.0 * kPi / 3.0));
    out[1] = 3.0 * q - out[0] - out[2];

    std::sort(out.begin(), out.end());
    return out;
}

std::vector<Principal3> PostProcessor::principalStresses(const std::vector<Voigt6>& stresses)
{
    std::vector<Principal3> out;
    out.reserve(stresses.size());

    for (const auto& stress : stresses) {
        out.push_back(principalStresses(stress));
    }

    return out;
}

std::size_t PostProcessor::resolveDofIndex(
    const DofMap& dofMap,
    std::size_t nodeId,
    std::size_t dof)
{
    return static_cast<std::size_t>(dofMap.dof(nodeId, dof));
}

Vector PostProcessor::recoverTet4ElementDisplacements(
    const LinearStaticResult& result,
    const DofMap& dofMap,
    const Element& element)
{
    if (element.node_ids.size() != 4) {
        throw std::invalid_argument("Tet4 element must have exactly 4 node IDs.");
    }

    Vector ue;
    ue.resize(12);

    for (std::size_t a = 0; a < 4; ++a) {
        const std::size_t nodeId = element.node_ids[a];

        for (std::size_t dof = 0; dof < 3; ++dof) {
            const std::size_t globalIndex = resolveDofIndex(dofMap, nodeId, dof);
            ue[3 * a + dof] = result.solution[globalIndex];
        }
    }

    return ue;
}

void PostProcessor::populateVectorField(
    VectorFieldData& field,
    const std::vector<Vector3>& values)
{
    field.values = values;

    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> zs;
    std::vector<double> mags;

    xs.reserve(values.size());
    ys.reserve(values.size());
    zs.reserve(values.size());
    mags.reserve(values.size());

    for (const auto& value : values) {
        xs.push_back(value[0]);
        ys.push_back(value[1]);
        zs.push_back(value[2]);
        mags.push_back(magnitude(value));
    }

    field.x.values = xs;
    field.y.values = ys;
    field.z.values = zs;
    field.magnitude.values = mags;

    field.x.summary = summarizeValues(field.x.values);
    field.y.summary = summarizeValues(field.y.values);
    field.z.summary = summarizeValues(field.z.values);
    field.magnitude.summary = summarizeValues(field.magnitude.values);
}

void PostProcessor::populateTensorField(
    TensorFieldData& field,
    const std::vector<Voigt6>& values)
{
    field.values = values;

    std::vector<double> xx;
    std::vector<double> yy;
    std::vector<double> zz;
    std::vector<double> xy;
    std::vector<double> yz;
    std::vector<double> zx;
    std::vector<double> mags;

    xx.reserve(values.size());
    yy.reserve(values.size());
    zz.reserve(values.size());
    xy.reserve(values.size());
    yz.reserve(values.size());
    zx.reserve(values.size());
    mags.reserve(values.size());

    for (const auto& value : values) {
        xx.push_back(value[0]);
        yy.push_back(value[1]);
        zz.push_back(value[2]);
        xy.push_back(value[3]);
        yz.push_back(value[4]);
        zx.push_back(value[5]);

        const double m = std::sqrt(
            value[0] * value[0] +
            value[1] * value[1] +
            value[2] * value[2] +
            2.0 * (value[3] * value[3] + value[4] * value[4] + value[5] * value[5]));
        mags.push_back(m);
    }

    field.xx.values = xx;
    field.yy.values = yy;
    field.zz.values = zz;
    field.xy.values = xy;
    field.yz.values = yz;
    field.zx.values = zx;
    field.magnitude.values = mags;

    field.xx.summary = summarizeValues(field.xx.values);
    field.yy.summary = summarizeValues(field.yy.values);
    field.zz.summary = summarizeValues(field.zz.values);
    field.xy.summary = summarizeValues(field.xy.values);
    field.yz.summary = summarizeValues(field.yz.values);
    field.zx.summary = summarizeValues(field.zx.values);
    field.magnitude.summary = summarizeValues(field.magnitude.values);
}

void PostProcessor::populateStressField(
    StressFieldData& field,
    const std::vector<Voigt6>& values)
{
    field.values = values;

    std::vector<double> xx;
    std::vector<double> yy;
    std::vector<double> zz;
    std::vector<double> xy;
    std::vector<double> yz;
    std::vector<double> zx;
    std::vector<double> mags;
    std::vector<double> hydro;
    std::vector<double> mises;
    std::vector<Principal3> principals;

    xx.reserve(values.size());
    yy.reserve(values.size());
    zz.reserve(values.size());
    xy.reserve(values.size());
    yz.reserve(values.size());
    zx.reserve(values.size());
    mags.reserve(values.size());
    hydro.reserve(values.size());
    mises.reserve(values.size());
    principals.reserve(values.size());

    for (const auto& value : values) {
        xx.push_back(value[0]);
        yy.push_back(value[1]);
        zz.push_back(value[2]);
        xy.push_back(value[3]);
        yz.push_back(value[4]);
        zx.push_back(value[5]);

        const double tensorMag = std::sqrt(
            value[0] * value[0] +
            value[1] * value[1] +
            value[2] * value[2] +
            2.0 * (value[3] * value[3] + value[4] * value[4] + value[5] * value[5]));
        mags.push_back(tensorMag);

        hydro.push_back(hydrostaticStress(value));
        mises.push_back(vonMises(value));
        principals.push_back(principalStresses(value));
    }

    field.xx.values = xx;
    field.yy.values = yy;
    field.zz.values = zz;
    field.xy.values = xy;
    field.yz.values = yz;
    field.zx.values = zx;
    field.magnitude.values = mags;
    field.hydrostatic.values = hydro;
    field.vonMises.values = mises;
    field.principal.values = principals;

    field.xx.summary = summarizeValues(field.xx.values);
    field.yy.summary = summarizeValues(field.yy.values);
    field.zz.summary = summarizeValues(field.zz.values);
    field.xy.summary = summarizeValues(field.xy.values);
    field.yz.summary = summarizeValues(field.yz.values);
    field.zx.summary = summarizeValues(field.zx.values);
    field.magnitude.summary = summarizeValues(field.magnitude.values);
    field.hydrostatic.summary = summarizeValues(field.hydrostatic.values);
    field.vonMises.summary = summarizeValues(field.vonMises.values);

    std::vector<double> minPrincipal;
    std::vector<double> midPrincipal;
    std::vector<double> maxPrincipal;
    minPrincipal.reserve(principals.size());
    midPrincipal.reserve(principals.size());
    maxPrincipal.reserve(principals.size());

    for (const auto& p : principals) {
        minPrincipal.push_back(p[0]);
        midPrincipal.push_back(p[1]);
        maxPrincipal.push_back(p[2]);
    }

    field.principal.minimum.values = minPrincipal;
    field.principal.middle.values = midPrincipal;
    field.principal.maximum.values = maxPrincipal;

    field.principal.minimum.summary = summarizeValues(field.principal.minimum.values);
    field.principal.middle.summary = summarizeValues(field.principal.middle.values);
    field.principal.maximum.summary = summarizeValues(field.principal.maximum.values);
}

PostProcessResult PostProcessor::process(
    const LinearStaticResult& result,
    const Mesh& mesh,
    const DofMap& dofMap,
    const std::vector<LinearElastic>& materials)
{
    PostProcessResult output;

    const auto& nodes = mesh.nodes();
    std::vector<Vector3> nodalDisplacements(nodes.size(), Vector3{0.0, 0.0, 0.0});

    for (const auto& node : nodes) {
        const std::size_t nodeId = static_cast<std::size_t>(node.id);
        if (nodeId >= nodalDisplacements.size()) {
            throw std::out_of_range("Node ID is out of range for displacement recovery.");
        }

        Vector3 displacement{0.0, 0.0, 0.0};
        for (std::size_t dof = 0; dof < 3; ++dof) {
            const std::size_t globalIndex = resolveDofIndex(dofMap, nodeId, dof);
            displacement[dof] = result.solution[globalIndex];
        }

        nodalDisplacements[nodeId] = displacement;
    }

    populateVectorField(output.displacement, nodalDisplacements);

    const auto& elements = mesh.elements();
    std::vector<Voigt6> elementStrains(elements.size(), Voigt6{0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
    std::vector<Voigt6> elementStresses(elements.size(), Voigt6{0.0, 0.0, 0.0, 0.0, 0.0, 0.0});

    for (std::size_t e = 0; e < elements.size(); ++e) {
        const Element& element = elements[e];

        if (element.type != ElementType::Tet4) {
            throw std::invalid_argument("PostProcessor currently supports Tet4 elements only.");
        }

        if (element.material_id >= materials.size()) {
            throw std::out_of_range("Element material_id is out of range.");
        }

        const Vector ue = recoverTet4ElementDisplacements(result, dofMap, element);
        const Vector eps = Tet4::strain(mesh, element, ue);
        const Vector sig = Tet4::stress(mesh, element, materials.at(element.material_id), ue);

        elementStrains[e] = vectorToVoigt(eps);
        elementStresses[e] = vectorToVoigt(sig);
    }

    populateTensorField(output.strain, elementStrains);
    populateStressField(output.stress, elementStresses);

    return output;
}

} // namespace feast
