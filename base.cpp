#include "base.h"

Matrix4x4 inverse(Matrix4x4 m) {
  int indxc[4], indxr[4], ipiv[4];
  int i, icol = 0, irow = 0, j, k, l, ll;
  double big, dum, pivinv, temp;
  double *temp2;
  for (j = 0; j < 4; ++j)
    ipiv[j] = 0;
  for (i = 0; i < 4; ++i) {
    big = 0.0;
    for (j = 0; j < 4; ++j) {
      if (ipiv[j] != 1) {
        for (k = 0; k < 4; ++k) {
          if (ipiv[k] == 0) {
            if (fabs(m.m[j][k]) >= big) {
              big = fabs(m.m[j][k]);
              irow = j;
              icol = k;
            }
          } else {
            if (ipiv[k] > 1) {
              return Matrix4x4();
            }
          }
        }
      }
    }
    ++ipiv[icol];
    if (irow != icol) {
      temp2 = m.m[irow];
      m.m[irow] = m.m[icol];
      m.m[icol] = temp2;
    }
    indxr[i] = irow;
    indxc[i] = icol;
    if (m.m[icol][icol] == 0.0) {
      return Matrix4x4();
    }
    pivinv = 1.0 / m.m[icol][icol];
    m.m[icol][icol] = 1.0;
    for (l = 0; l < 4; ++l)
      m.m[icol][l] *= pivinv;
    for (ll = 0; ll < 4; ++ll) {
      if (ll != icol) {
        dum = m.m[ll][icol];
        m.m[ll][icol] = 0.0;
        for (l = 0; l < 4; ++l)
          m.m[ll][l] -= m.m[icol][l] * dum;
      }
    }
  }
  for (l = 3; l >= 0; --l) {
    if (indxr[l] != indxc[l]) {
      for (k = 0; k < 4; ++k) {
        temp = m.m[k][indxr[l]];
        m.m[k][indxr[l]] = m.m[k][indxc[l]];
        m.m[k][indxc[l]] = temp;
      }
    }
  }

  return m;
}


std::vector<double> flatten(Matrix4x4 m) {
  std::vector<double> v;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      v.push_back(m.m[i][j]);
    }
  }
  return v;
}