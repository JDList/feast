#pragma once


namespace feast
{

class Cuboid
{
public:

    Cuboid(
        double lx,
        double ly,
        double lz);

    double lengthX() const;
    double lengthY() const;
    double lengthZ() const;
    
private:

    double lx_;
    double ly_;
    double lz_;

};

}
