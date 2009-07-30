#ifndef _PAIRS_COMPUTER_HPP
#define _PAIRS_COMPUTER_HPP

#include "types.hpp"
#include "storage/ParticleHandle.hpp"
#include "storage/Storage.hpp"
#include "boost/function.hpp"

namespace espresso {
  namespace pairs {
    /** Abstract class that defines a function on pairs of particles */
    class Computer {
    public:
      typedef shared_ptr< Computer > SelfPtr;

      virtual ~Computer() {}

      /// @name extended function object interface
      //@{
      typedef Real3D first_argument_type;
      typedef storage::ParticleHandle second_argument_type;
      typedef storage::ParticleHandle  third_argument_type;
      typedef void                       result_type;
      //@}

      virtual void prepare(storage::Storage::SelfPtr storage1, 
			   storage::Storage::SelfPtr storage2) = 0;

      /** Interface of the routine that is applied to particle pairs
	  \param dist: distance vector between the two particles
	  \param p1, p2: references to the two particles

	  The pair computer gets the distance vector between the
	  particles, as it is hard to get otherwise and furthermore
	  some pair generators compute it anyway.

	  Note: The references are necessary if more property data of
	  the particles is needed than only the distance.
      */
      virtual void apply(const Real3D dist, 
			 const storage::ParticleHandle p1, 
			 const storage::ParticleHandle p2) = 0;

      virtual void finalize() {}

      static void registerPython();
    };
  }
}

#endif
