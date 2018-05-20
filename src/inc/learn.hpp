#ifndef LEARN_INCLUDED
#define LEARN_INCLUDED

#include "data_io.hpp"
#include "poscomp.h"
#include <dlib/dnn.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace dlib;

template <typename net_type, typename sample_type>
double get_net_accuracy(net_type &net, std::vector<sample_type> &samples,
                        std::vector<float> &labels) {
  std::vector<float> predicted_labels = net(samples);
  int num_right = 0;
  int num_wrong = 0;

  for (size_t i = 0; i < samples.size(); ++i) {
    if ((predicted_labels[i] > 0 && labels[i] > 0) ||
        (predicted_labels[i] < 0 && labels[i] < 0))
      ++num_right;
    else
      ++num_wrong;
  }

  return num_right * 100.0 / (double)(num_right + num_wrong);
}

void train(std::istream &in, std::ostream &out) {
  std::vector<sample_type> train_samples, test_samples;
  std::vector<float> train_labels, test_labels;

  load_train_test(in, 10, train_samples, train_labels, test_samples,
                  test_labels);

  net_type net;

  dnn_trainer<net_type, adam> trainer(net, adam(0.0, 0.9, 0.999));
  trainer.set_learning_rate(2e-4);
  trainer.set_learning_rate_shrink_factor(1);
  trainer.set_min_learning_rate(1e-10);
  trainer.set_mini_batch_size(8);

  std::cout.precision(2);

  for (int e = 10; e < 1000; e += 10) {
    trainer.set_max_num_epochs(e);
    trainer.train(train_samples, train_labels);
    std::cout << " --- Epochs:" << std::setw(6) << e
              << "    train acc: " << std::fixed
              << get_net_accuracy<net_type, sample_type>(net, train_samples,
                                                         train_labels)
              << "    test acc: " << std::fixed
              << get_net_accuracy<net_type, sample_type>(net, test_samples,
                                                         test_labels)
              << std::endl;
  }

  trainer.get_net();

  net.clean();

  serialize(net, out);
}

#endif // #ifndef LEARN_INCLUDED
