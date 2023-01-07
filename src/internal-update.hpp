#pragma once
#include "pch.hpp"


class InternalUpdate
{
  int updatesInProgress_ = 0;

public:
  bool isUpdateInProgress() const { return updatesInProgress_ != 0; }
  void onUpdate() { ++updatesInProgress_; }
  void onUpdateFinished() { --updatesInProgress_; }
};


class UpdateInProgress
{
  InternalUpdate& internalUpdate_;

public:
  UpdateInProgress( InternalUpdate& internalUpdate )
      : internalUpdate_{ internalUpdate }
  {
    internalUpdate_.onUpdate();
  }

  ~UpdateInProgress()
  {
    internalUpdate_.onUpdateFinished();
  }
};
