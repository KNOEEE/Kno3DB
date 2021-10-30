#include "histogram.h"

void PopulateHistogram(Histogram& histogram,
             uint64_t low, uint64_t high, uint64_t loop = 1) {
  for (; loop > 0; loop--) {
    for (uint64_t i = low; i <= high; i++) {
      histogram.Add(i);
      usleep(1000);
    }
  }
  // make sure each data population at least take some time
  // 10ms
  usleep(10000);
}

int main () {
  bucketMapper.PrintBucketValues();
  HistogramImpl histogram;
  PopulateHistogram(histogram, 1, 10, 10);
  histogram.PrintStatBucket();
  std::cout << histogram.Percentile(100.0) << std::endl;
  std::cout << histogram.Percentile(99.0) << std::endl;
  std::cout << histogram.Percentile(95.0) << std::endl;
  std::cout << histogram.Median() << std::endl;
  std::cout << histogram.Average() << std::endl;
}
