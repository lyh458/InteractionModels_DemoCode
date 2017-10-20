#include <boost/chrono.hpp>

using namespace boost::chrono;

namespace iMod{
/*!
 * @brief implements a simple Timer_T
 *  
 * <CODE> <br> 
 *    Timer_T<system_clock> t1;
 *    Timer_T<steady_clock> t2;
 *    Timer_T<high_resolution_clock> t3;
 *
 *  std::cout << std::fixed << std::setprecision(9);
 *  std::cout << "system_clock-----------: "
 *            << t1.seconds() << " seconds\n";
 *  std::cout << "steady_clock--------: "
 *            << t2.seconds() << " seconds\n";
 *  std::cout << "high_resolution_clock--: "
 *            << t3.seconds() << " seconds\n";
 *</CODE>
 */
  template< class Clock >
  class Timer_T
  {
    typename Clock::time_point start;
  public:
    Timer_T() : start( Clock::now() ) {}
    typename Clock::duration elapsed() const
    {
      return Clock::now() - start;
    }
    double seconds() const
    {
      return elapsed().count() * ((double)Clock::period::num/Clock::period::den);
    }
  };
}