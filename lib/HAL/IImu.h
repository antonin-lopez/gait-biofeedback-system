#ifndef IIMU_H
#define IIMU_H

class IImu {
public:
    virtual ~IImu() = default;
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual float getAccelerationZ() = 0;
    virtual float getAccelerationMagnitude() = 0;
};

#endif // IIMU_H
