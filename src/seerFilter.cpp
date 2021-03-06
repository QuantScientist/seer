/*
 * File: seerFilter.cpp
 *
 * Filters kmers that won't be meaningfully associated.
 * Saves time over performing regressions on all, and reduces false positives
 *
 */

#include "seercommon.hpp"

// Wrapper to all filter functions
int passFilters(const cmdOptions& filterOptions, Kmer& k, const std::vector<Sample>& samples, const arma::vec& y, const int continuous_phenotype, const bool do_stats)
{
   int pass = 0;

   if (passBasicFilters(k, filterOptions.max_length, filterOptions.min_words, filterOptions.max_words))
   {
      // Don't bother with this if not running stats tests
      pass = 1;

      if (do_stats)
      {
         try  // Some chi^2 tests may diverge - proceed anyway for now
         {
            pass = passStatsFilters(k, y, filterOptions.chi_cutoff, continuous_phenotype);
         }
         catch (std::exception& e)
         {
            std::cerr << "kmer " + k.sequence() + " failed chisq test with error: " + e.what() + "\n";
            pass = 1;
         }
      }
   }

   return pass;
}

int passBasicFilters(const Kmer& k, const int max_length, const int min_words, const int max_words)
{
   int passed = 1;

   // Don't test long kmers
   if (k.length() > max_length)
   {
      passed = 0;
   }

   // Impose min words
   // TODO may want to make this more sophisticated, make sure there are at
   // least ten words in each category
   if (passed && (k.num_occurrences() < min_words || k.num_occurrences() > max_words))
   {
      passed = 0;
   }

   return passed;
}

int passStatsFilters(Kmer& k, const arma::vec& y, const double chi_cutoff, const int continuous_phenotype)
{
   int passed = 1;

   if (continuous_phenotype)
   {
      k.unadj_p_val(welchTwoSamplet(k, y));
   }
   else
   {
      k.unadj_p_val(chiTest(k, y));
   }

   if (k.unadj() > chi_cutoff)
   {
     passed = 0;
   }

   return passed;
}

