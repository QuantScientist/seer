/*
 * File: significant_kmer.cpp
 *
 * Helper functions for the significant kmer class
 * Reads input into class
 *
 */

#include "significant_kmer.hpp"

Significant_kmer::Significant_kmer()
{
}

Significant_kmer::Significant_kmer(const std::string& word, const std::vector<std::string>& samples, const double maf, const double unadj_p, const double adj_p, const double beta, const double se, const std::string& comments)
   :_word(word), _samples(samples), _maf(maf), _unadj_p(unadj_p), _adj_p(adj_p), _beta(beta), _se(se), _comment(comments)
{
}

// Fills significant kmer object from seer output file
// Sample vector is returned sorted
std::istream& operator>>(std::istream &is, Significant_kmer& sk)
{
   double maf, unadj_p, adj_p, beta, se;
   std::string sequence, sample, line_in, comments = "";
   std::vector<std::string> sample_list;

   // Read the line convert to stringstream, extract sequence and stats fields
   std::getline(is, line_in);
   std::stringstream line_stream(line_in);

   line_stream >> sequence;

   line_stream >> maf;
   line_stream >> unadj_p;
   line_stream >> adj_p;
   line_stream >> beta;
   line_stream >> se;

   line_stream >> comments;

   // Read remainder of line, which is sample names, in the same way they were
   // written
   std::copy(std::istream_iterator<std::string>(line_stream), std::istream_iterator<std::string>(), std::back_inserter(sample_list));

   // Ensure vector remains sorted on sample name
   std::sort(sample_list.begin(), sample_list.end());
   sk = Significant_kmer(sequence, sample_list, maf, unadj_p, adj_p, beta, se, comments);

   return is;
}

// Print fields tab sep, identical to input. Doesn't print newline
std::ostream& operator<<(std::ostream &os, const Significant_kmer& k)
{
   os << k.sequence() << "\t"
      << std::fixed << std::setprecision(3) << k.maf() << "\t"
      << std::scientific << k.unadj() << "\t" << k.p_val() << "\t" << k.beta() << "\t" << k.se()
      << "\t" << k.comments() << "\t";

   std::vector<std::string> samples_found = k.samples_found();
   std::copy(samples_found.begin(), samples_found.end() - 1, std::ostream_iterator<std::string>(os, "\t"));
   os << samples_found.back();

   return os;
}

sortSigKmer::sortSigKmer()
   :_sort_field(0)
{
}

sortSigKmer::sortSigKmer(const std::string& sort_field)
   :_sort_field(0)
{
   this->addSortField(sort_field);
}


// Sets the sort field for sig kmers
void sortSigKmer::addSortField(const std::string& sort_field)
{
   if (sort_field == "maf")
   {
      this->_sort_field = 1;
   }
   else if (sort_field == "chisq")
   {
      this->_sort_field = 2;
   }
   else if (sort_field == "pval")
   {
      this->_sort_field = 3;
   }
   else if (sort_field == "beta")
   {
      this->_sort_field = 4;
   }
   else if (sort_field == "sequence")
   {
      this->_sort_field = 5;
   }
   else
   {
      this->_sort_field = 3;
   }
}

bool sortSigKmer::operator() (const Significant_kmer& sk1, const Significant_kmer& sk2) const
{
   bool compare;
   switch (_sort_field)
   {
      case 1:
         compare = sk1.maf() < sk2.maf();
         break;
      case 2:
         compare = sk1.unadj() < sk2.unadj();
         break;
      case 3:
         compare = sk1.p_val() < sk2.p_val();
         break;
      case 4:
         compare = sk1.beta() < sk2.beta();
         break;
      case 5:
         compare = sk1.sequence().length() < sk2.sequence().length();
         break;
      default:
         compare = sk1.p_val() < sk2.p_val();
   }
   return compare;
}
