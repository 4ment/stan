#ifndef __STAN__MCMC__DIAG__METRO__HPP
#define __STAN__MCMC__DIAG__METRO__HPP

#include <stan/mcmc/metro/base_metro.hpp>
#include <stan/prob/distributions/univariate/continuous/normal.hpp>

namespace stan {

  namespace mcmc {
    
    template <typename M, class BaseRNG>
    class diag_metro: public base_metro<M, BaseRNG> {
      
    public:
      
      diag_metro(M& m, 
                 BaseRNG& rng, 
                 std::ostream* o = &std::cout, 
                 std::ostream* e = 0)
        : base_metro<M, BaseRNG>(m, rng, o, e),
          _prop_cov_diag(Eigen::VectorXd::Ones(m.num_params_r())) { 
        this->_name = "Metropolis with a diagonal metric"; 
        this->_nom_epsilon = 1;
      }

      void propose(std::vector<double>& q,
                    BaseRNG& rng) {

        for (size_t i = 0; i < q.size(); ++i) 
          q[i] = stan::prob::normal_rng(q[i],this->_nom_epsilon 
                                        * _prop_cov_diag(i), this->_rand_int);

        try {
          this->_log_prob = this->log_prob(q, this->_params_i);
        } catch (std::domain_error e) {
          this->_write_error_msg(this->_err_stream, e);
          this->_log_prob = -std::numeric_limits<double>::infinity();
        }
      }

      void write_metric(std::ostream& o) {
        o << "# Diagonal elements of covariance matrix:" << std::endl;
        o << "# " << _prop_cov_diag(0) << std::flush;
        for(size_t i = 1; i < _prop_cov_diag.size(); ++i)
          o << ", " << _prop_cov_diag(i) << std::flush;
        o << std::endl;
      };
             
    protected:

      Eigen::VectorXd _prop_cov_diag;           
    };

  } // mcmc

} // stan
          

#endif