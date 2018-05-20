#ifndef DATA_IO_INCLUDED
#define DATA_IO_INCLUDED

#include <dlib/matrix.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace dlib;

template <typename sample_type>
void load_train_test(std::istream &in, float test_perc,
                     std::vector<sample_type> &train_samples,
                     std::vector<float> &train_labels,
                     std::vector<sample_type> &test_samples,
                     std::vector<float> &test_labels) {
  dlib::rand rnd(42);

  sample_type st;
  unsigned feat_count = st.nr();

  std::string line;

  while (getline(in, line)) {
    std::vector<std::string> line_cells;

    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
      line_cells.push_back(cell);
    }

    sample_type sample;

    for (unsigned i = 0; i < feat_count; ++i) {
      sample(i) = atof(line_cells[i].c_str());
    }

    float label;
    unsigned c = feat_count;

    if (line_cells[c] == "Left")
      label = +1;
    else if (line_cells[c] == "Right")
      label = -1;
    else
      assert(false);

    if (rnd.get_random_32bit_number() % 100 > test_perc) {
      train_samples.push_back(sample);
      train_labels.push_back(label);
    } else {
      test_samples.push_back(sample);
      test_labels.push_back(label);
    }
  }

  randomize_samples(train_samples, train_labels);
  randomize_samples(test_samples, test_labels);

  std::cout << "Training sample size: " << train_samples.size() << std::endl
            << "Test sample size: " << test_samples.size() << std::endl;
}

#endif // #ifndef DATA_IO_INCLUDED
