#include "mts/audio/detail/vector_operations_vdsp.h"

#if __MTS_USE_ACCELERATE__
  #include "mts/assert.h"
  #include <Accelerate/Accelerate.h>

MTS_BEGIN_SUB_NAMESPACE(vec)

void vdsp_ops::clear(float* sd, stride_t s_sd, length_t length) { vDSP_vclr(sd, s_sd, length); }

void vdsp_ops::clear(double* sd, stride_t s_sd, length_t length) { vDSP_vclrD(sd, s_sd, length); }

void vdsp_ops::fill(float* sd, stride_t s_sd, float value, length_t length) { vDSP_vfill(&value, sd, s_sd, length); }

void vdsp_ops::fill(double* sd, stride_t s_sd, double value, length_t length) { vDSP_vfillD(&value, sd, s_sd, length); }

void vdsp_ops::copy(const float* s1, stride_t s_s1, float* d1, stride_t s_d1, length_t length) {
  cblas_scopy((int)length, s1, (int)s_s1, d1, (int)s_d1);
}

void vdsp_ops::copy(const double* s1, stride_t s_s1, double* d1, stride_t s_d1, length_t length) {
  cblas_dcopy((int)length, s1, (int)s_s1, d1, (int)s_d1);
}

void vdsp_ops::clip(const float* s1, stride_t s_s1, float min, float max, float* d1, stride_t s_d1, length_t length) {
  vDSP_vclip(s1, s_s1, &min, &max, d1, s_d1, length);
}

void vdsp_ops::clip(
    const double* s1, stride_t s_s1, double min, double max, double* d1, stride_t s_d1, length_t length) {
  vDSP_vclipD(s1, s_s1, &min, &max, d1, s_d1, length);
}

void vdsp_ops::abs(const float* s1, stride_t s_s1, float* d1, stride_t s_d1, length_t length) {
  vDSP_vabs(s1, s_s1, d1, s_d1, length);
}

void vdsp_ops::abs(const double* s1, stride_t s_s1, double* d1, stride_t s_d1, length_t length) {
  vDSP_vabsD(s1, s_s1, d1, s_d1, length);
}

void vdsp_ops::add(const float* s1, stride_t s_s1, float value, float* d1, stride_t s_d1, length_t length) {
  vDSP_vsadd(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::add(const double* s1, stride_t s_s1, double value, double* d1, stride_t s_d1, length_t length) {
  vDSP_vsaddD(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::mul(const float* s1, stride_t s_s1, float value, float* d1, stride_t s_d1, length_t length) {
  vDSP_vsmul(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::mul(const double* s1, stride_t s_s1, double value, double* d1, stride_t s_d1, length_t length) {
  vDSP_vsmulD(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::div(const float* s1, stride_t s_s1, float value, float* d1, stride_t s_d1, length_t length) {
  vDSP_vsdiv(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::div(const double* s1, stride_t s_s1, double value, double* d1, stride_t s_d1, length_t length) {
  vDSP_vsdivD(s1, s_s1, &value, d1, s_d1, length);
}

void vdsp_ops::vdiv(float value, const float* s1, stride_t s_s1, float* d1, stride_t s_d1, length_t length) {
  vDSP_svdiv(&value, s1, s_s1, d1, s_d1, length);
}

void vdsp_ops::vdiv(double value, const double* s1, stride_t s_s1, double* d1, stride_t s_d1, length_t length) {
  vDSP_svdivD(&value, s1, s_s1, d1, s_d1, length);
}

void vdsp_ops::sin(const float* s1, float* d1, length_t length) {
  int i_size = (int)length;
  vvsinf(d1, s1, &i_size);
}

void vdsp_ops::sin(const double* s1, double* d1, length_t length) {
  int i_size = (int)length;
  vvsin(d1, s1, &i_size);
}

void vdsp_ops::cos(const float* s1, float* d1, length_t length) {
  int i_size = (int)length;
  vvcosf(d1, s1, &i_size);
}

void vdsp_ops::cos(const double* s1, double* d1, length_t length) {
  int i_size = (int)length;
  vvcos(d1, s1, &i_size);
}

void vdsp_ops::tan(const float* s1, float* d1, length_t length) {
  int i_size = (int)length;
  vvtanf(d1, s1, &i_size);
}

void vdsp_ops::tan(const double* s1, double* d1, length_t length) {
  int i_size = (int)length;
  vvtan(d1, s1, &i_size);
}

void vdsp_ops::tanh(const float* s1, float* d1, length_t length) {
  int i_size = (int)length;
  vvtanhf(d1, s1, &i_size);
}

void vdsp_ops::tanh(const double* s1, double* d1, length_t length) {
  int i_size = (int)length;
  vvtanh(d1, s1, &i_size);
}
//

MTS_END_SUB_NAMESPACE(vec)
#endif // __MTS_USE_ACCELERATE__
