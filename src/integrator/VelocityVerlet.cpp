#include "VelocityVerlet.hpp"
#include <boost/foreach.hpp>
#include <python.hpp>
#include "particles/Computer.hpp"

using namespace espresso;
using namespace espresso::integrator;
using namespace espresso::particles;
using namespace espresso::storage;
using namespace boost;

namespace {
  // fire once computer that should not be reused
  struct StepA : public Computer {
    const PropertyHandle< Real3D > pos;
    const PropertyHandle< Real3D > vel;
    const PropertyHandle< Real3D > force;

    const real timestep;
    const real timestepSqr;

    StepA(Set::SelfPtr set,
	  Property< Real3D >::SelfPtr posProperty,
	  Property< Real3D >::SelfPtr velProperty,
	  Property< Real3D >::SelfPtr forceProperty, 
	  real _timestep)
      : pos(posProperty->getHandle(set)), 
	vel(velProperty->getHandle(set)), 
	force(forceProperty->getHandle(set)),
	timestep(_timestep), 
	timestepSqr(_timestep * _timestep) 
    {}

    void prepare(Storage::SelfPtr set) {}
  
    void apply(const ParticleHandle pref) {
      pos[pref] = pos[pref] + vel[pref] * timestep + 0.5 * force[pref] * timestepSqr;
      vel[pref] = vel[pref] + 0.5 * force[pref] * timestep;
    }
  };

  struct StepB : public Computer {
    const PropertyHandle< Real3D > vel;
    const PropertyHandle< Real3D > force;

    const real timestep;

    StepB(Set::SelfPtr set,
	  Property< Real3D >::SelfPtr velProperty,
	  Property< Real3D >::SelfPtr forceProperty, 
	  real _timestep)
      : vel(velProperty->getHandle(set)), 
	force(forceProperty->getHandle(set)),
	timestep(_timestep)
    {}
    
    void prepare(Storage::SelfPtr set) {}

    void apply(const ParticleHandle pref) { 
      vel[pref] = vel[pref] + 0.5 * force[pref] * timestep; 
    }
  };

  struct StepZeroForces: public Computer {
    const PropertyHandle< Real3D > force;

    StepZeroForces(Set::SelfPtr set,
		   Property< Real3D >::SelfPtr forceProperty)
      : force(forceProperty->getHandle(set)) {}
    
    void prepare(Storage::SelfPtr set) {}
    void apply(const ParticleHandle pref) {
      force[pref] = 0.0;
    }
  };
}

VelocityVerlet::VelocityVerlet(Set::SelfPtr set, 
                               Property< Real3D >::SelfPtr posProperty,
                               Property< Real3D >::SelfPtr velProperty,
                               Property< Real3D >::SelfPtr forceProperty,
			       real _timestep)
  : MDIntegrator(set, posProperty, velProperty, forceProperty, _timestep) 
{}

void VelocityVerlet::step() {
  StepA stepA(set, posProperty, velProperty, forceProperty, timestep);

  // Step A
  set->foreach(stepA);

  // call connected routines, e.g. thermalizeA for a chosen thermostat
  updateVelocity1(*this);

  // set forces to ZERO after calling updateVelocity1
  StepZeroForces stepZeroForces(set, forceProperty);
  set->foreach(stepZeroForces);

  // calculate forces:
  updateForces(*this);

  // Step B
  StepB stepB(set, velProperty, forceProperty, timestep);
  set->foreach(stepB);

  // call connected routines, e.g. thermalizeB for a chosen thermostat
  updateVelocity2(*this);
}

VelocityVerlet::~VelocityVerlet() {}

//////////////////////////////////////////////////
// REGISTRATION WITH PYTHON
//////////////////////////////////////////////////

void
VelocityVerlet::registerPython() {
  using namespace boost;
  using namespace espresso::python;

  // TODO: Why noncopyable?
  class_< VelocityVerlet, boost::noncopyable, bases< MDIntegrator > >
    ("integrator_VelocityVerlet", 
     init< 
     Set::SelfPtr, 
     Property< Real3D >::SelfPtr, 
     Property< Real3D >::SelfPtr, 
     Property< Real3D >::SelfPtr,
     real
     >())
    ;
}
