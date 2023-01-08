#pragma once
#include "pch.hpp"


template<typename TResult>
class ThreadPool
{
public:
  using Func = std::function<TResult()>;

private:
  std::vector<Func> functors_;
  std::mutex        functorMutex_;

  std::vector<TResult> results_;
  std::mutex           resultsMutex_;

  std::atomic<uint32_t>    jobCount_;
  std::vector<std::thread> threads_;

  std::latch beginBarrier_;

public:
  explicit ThreadPool( std::vector<Func> functors )
      : functors_{ std::move( functors ) }
      , jobCount_{ ( uint32_t ) functors_.size() }
      , beginBarrier_{ 1 }
  {
    results_.reserve( functors_.size() );
  }

  ~ThreadPool()
  {
    // TODO: cancel remaining functors
    waitAndExit();
  }

  void start()
  {
    if( !threads_.empty() )
      return;

    uint32_t numThreads = std::max( std::thread::hardware_concurrency(), 8u );
    threads_.reserve( numThreads );

    for( uint32_t i = 0; i < numThreads; ++i )
      threads_.emplace_back( threadFunc, this );

    beginBarrier_.count_down();
  }

  // TODO: cancel function

  void waitAndExit()
  {
    if( threads_.empty() )
      return;

    for( auto& thread: threads_ )
      thread.join();

    threads_.clear();
  }

  uint32_t             getJobRemaining() const { return jobCount_; }
  std::vector<TResult> getResults() const { return results_; }

private:
  static void threadFunc( ThreadPool* pool )
  {
    pool->beginBarrier_.wait();

    while( true )
    {
      Func func;

      {
        auto lock = std::unique_lock{ pool->functorMutex_ };

        if( pool->functors_.empty() )
          return;

        func = pool->functors_.back();
        pool->functors_.pop_back();
      }

      auto result = func();

      {
        auto lock = std::unique_lock{ pool->resultsMutex_ };
        pool->results_.emplace_back( std::move( result ) );
      }

      --pool->jobCount_;
    }
  }
};
