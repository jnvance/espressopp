#include "All.hpp"

using namespace espresso::particles;

All::All(boost::shared_ptr<Storage> _store) : Set(_store) {}

All::~All() {}

bool All::isMember(ParticleHandle) const { return true; }

void All::foreach(Computer &computer) {
  if (theStorage) theStorage->foreach(computer);
}

void All::foreach(ConstComputer &computer) const {
  if (theStorage) theStorage->foreach(computer);
}

