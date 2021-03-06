/// autogenerated analytical inverse kinematics code from ikfast program part of OpenRAVE
/// \author Rosen Diankov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// ikfast version 45 generated on 2012-01-11 11:51:27.791002
/// To compile with gcc:
///     gcc -lstdc++ ik.cpp
/// To compile without any main function as a shared object:
///     gcc -fPIC -lstdc++ -DIKFAST_NO_MAIN -shared -Wl,-soname,ik.so -o ik.so ik.cpp
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <complex>

#ifdef IKFAST_HEADER
#include IKFAST_HEADER
#endif

#ifndef IKFAST_ASSERT
#include <stdexcept>
#include <sstream>

#ifdef _MSC_VER
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

#define IKFAST_ASSERT(b) { if( !(b) ) { std::stringstream ss; ss << "ikfast exception: " << __FILE__ << ":" << __LINE__ << ": " <<__PRETTY_FUNCTION__ << ": Assertion '" << #b << "' failed"; throw std::runtime_error(ss.str()); } }

#endif

#if defined(_MSC_VER)
#define IKFAST_ALIGNED16(x) __declspec(align(16)) x
#else
#define IKFAST_ALIGNED16(x) x __attribute((aligned(16)))
#endif

#define IK2PI  ((IKReal)6.28318530717959)
#define IKPI  ((IKReal)3.14159265358979)
#define IKPI_2  ((IKReal)1.57079632679490)

#ifdef _MSC_VER
#ifndef isnan
#define isnan _isnan
#endif
#endif // _MSC_VER

// defined when creating a shared object/dll
#ifdef IKFAST_CLIBRARY
#ifdef _MSC_VER
#define IKFAST_API extern "C" __declspec(dllexport)
#else
#define IKFAST_API extern "C"
#endif
#else
#define IKFAST_API
#endif

// lapack routines
extern "C" {
  void dgetrf_ (const int* m, const int* n, double* a, const int* lda, int* ipiv, int* info);
  void zgetrf_ (const int* m, const int* n, std::complex<double>* a, const int* lda, int* ipiv, int* info);
  void dgetri_(const int* n, const double* a, const int* lda, int* ipiv, double* work, const int* lwork, int* info);
  void dgesv_ (const int* n, const int* nrhs, double* a, const int* lda, int* ipiv, double* b, const int* ldb, int* info);
  void dgetrs_(const char *trans, const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, int *info);
  void dgeev_(const char *jobvl, const char *jobvr, const int *n, double *a, const int *lda, double *wr, double *wi,double *vl, const int *ldvl, double *vr, const int *ldvr, double *work, const int *lwork, int *info);
}

using namespace std; // necessary to get std math routines

#ifdef IKFAST_NAMESPACE
namespace IKFAST_NAMESPACE {
#endif

#ifdef IKFAST_REAL
typedef IKFAST_REAL IKReal;
#else
typedef double IKReal;
#endif

class IKSolution
{
public:
    /// Gets a solution given its free parameters
    /// \param pfree The free parameters required, range is in [-pi,pi]
    void GetSolution(IKReal* psolution, const IKReal* pfree) const {
        for(std::size_t i = 0; i < basesol.size(); ++i) {
            if( basesol[i].freeind < 0 )
                psolution[i] = basesol[i].foffset;
            else {
                IKFAST_ASSERT(pfree != NULL);
                psolution[i] = pfree[basesol[i].freeind]*basesol[i].fmul + basesol[i].foffset;
                if( psolution[i] > IKPI ) {
                    psolution[i] -= IK2PI;
                }
                else if( psolution[i] < -IKPI ) {
                    psolution[i] += IK2PI;
                }
            }
        }
    }

    /// Gets the free parameters the solution requires to be set before a full solution can be returned
    /// \return vector of indices indicating the free parameters
    const std::vector<int>& GetFree() const { return vfree; }

    struct VARIABLE
    {
        VARIABLE() : freeind(-1), fmul(0), foffset(0) {}
        VARIABLE(int freeind, IKReal fmul, IKReal foffset) : freeind(freeind), fmul(fmul), foffset(foffset) {}
        int freeind;
        IKReal fmul, foffset; ///< joint value is fmul*sol[freeind]+foffset
    };

    std::vector<VARIABLE> basesol;       ///< solution and their offsets if joints are mimiced
    std::vector<int> vfree;
};

inline float IKabs(float f) { return fabsf(f); }
inline double IKabs(double f) { return fabs(f); }

inline float IKlog(float f) { return logf(f); }
inline double IKlog(double f) { return log(f); }

#ifndef IKFAST_SINCOS_THRESH
#define IKFAST_SINCOS_THRESH ((IKReal)0.000001)
#endif

inline float IKasin(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asinf(f);
}
inline double IKasin(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asin(f);
}

// return positive value in [0,y)
inline float IKfmod(float x, float y)
{
    while(x < 0) {
        x += y;
    }
    return fmodf(x,y);
}

// return positive value in [0,y)
inline float IKfmod(double x, double y)
{
    while(x < 0) {
        x += y;
    }
    return fmod(x,y);
}

inline float IKacos(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0;
return acosf(f);
}
inline double IKacos(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0;
return acos(f);
}
inline float IKsin(float f) { return sinf(f); }
inline double IKsin(double f) { return sin(f); }
inline float IKcos(float f) { return cosf(f); }
inline double IKcos(double f) { return cos(f); }
inline float IKtan(float f) { return tanf(f); }
inline double IKtan(double f) { return tan(f); }
inline float IKsqrt(float f) { if( f <= 0.0f ) return 0.0f; return sqrtf(f); }
inline double IKsqrt(double f) { if( f <= 0.0 ) return 0.0; return sqrt(f); }
inline float IKatan2(float fy, float fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2f(fy,fx);
}
inline double IKatan2(double fy, double fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2(fy,fx);
}

inline float IKsign(float f) {
    if( f > 0 ) {
        return 1.0f;
    }
    else if( f < 0 ) {
        return -1.0f;
    }
    return 0;
}

inline double IKsign(double f) {
    if( f > 0 ) {
        return 1.0;
    }
    else if( f < 0 ) {
        return -1.0;
    }
    return 0;
}

/// solves the forward kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API void fk(const IKReal* j, IKReal* eetrans, IKReal* eerot) {
IKReal x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50;
x0=IKcos(j[0]);
x1=IKcos(j[1]);
x2=IKsin(j[2]);
x3=IKcos(j[2]);
x4=IKsin(j[1]);
x5=IKcos(j[4]);
x6=IKcos(j[3]);
x7=((x0)*(x1)*(x2));
x8=((x0)*(x3)*(x4));
x9=((x7)+(x8));
x10=((-1.00000000000000)*(x9));
x11=((x10)*(x6));
x12=IKsin(j[0]);
x13=IKsin(j[3]);
x14=((x12)*(x13));
x15=((((-1.00000000000000)*(x14)))+(x11));
x16=IKsin(j[4]);
x17=((x0)*(x2)*(x4));
x18=((x0)*(x1)*(x3));
x19=IKcos(j[5]);
x20=((x15)*(x5));
x21=((((-1.00000000000000)*(x18)))+(x17));
x22=((x16)*(x21));
x23=((x20)+(x22));
x24=((-1.00000000000000)*(x23));
x25=IKsin(j[5]);
x26=((x12)*(x6));
x27=((x13)*(x9));
x28=((x1)*(x12)*(x2));
x29=((x12)*(x3)*(x4));
x30=((x28)+(x29));
x31=((x12)*(x2)*(x4));
x32=((x1)*(x12)*(x3));
x33=((x0)*(x13));
x34=((-1.00000000000000)*(x30));
x35=((x34)*(x6));
x36=((x33)+(x35));
x37=((x13)*(x30));
x38=((x0)*(x6));
x39=((x38)+(x37));
x40=((((-1.00000000000000)*(x32)))+(x31));
x41=((x16)*(x40));
x42=((x36)*(x5));
x43=((x42)+(x41));
x44=((-1.00000000000000)*(x43));
x45=((x1)*(x2));
x46=((x3)*(x4));
x47=((x46)+(x45));
x48=((x2)*(x4));
x49=((x1)*(x3));
x50=((((-1.00000000000000)*(x49)))+(x48));
eerot[0]=((((x5)*(((((-1.00000000000000)*(x17)))+(x18)))))+(((x15)*(x16))));
eerot[1]=((((x19)*(((((-1.00000000000000)*(x26)))+(x27)))))+(((x24)*(x25))));
eerot[2]=((((x19)*(x24)))+(((x25)*(((((-1.00000000000000)*(x27)))+(x26))))));
eetrans[0]=((((0.270000000000000)*(x0)*(x4)))+(((x5)*(((((0.0800000000000000)*(x18)))+(((-0.0800000000000000)*(x17)))))))+(((0.295000000000000)*(x18)))+(((0.0740000000000000)*(x0)))+(((-0.295000000000000)*(x17)))+(((0.0900000000000000)*(x7)))+(((0.0900000000000000)*(x8)))+(((x16)*(((((0.0800000000000000)*(x11)))+(((-0.0800000000000000)*(x14))))))));
eerot[3]=((((x5)*(((((-1.00000000000000)*(x31)))+(x32)))))+(((x16)*(x36))));
eerot[4]=((((x19)*(x39)))+(((x25)*(x44))));
eerot[5]=((((x19)*(x44)))+(((-1.00000000000000)*(x25)*(x39))));
eetrans[1]=((((0.295000000000000)*(x32)))+(((-0.295000000000000)*(x31)))+(((0.0900000000000000)*(x29)))+(((0.0900000000000000)*(x28)))+(((0.270000000000000)*(x12)*(x4)))+(((x16)*(((((0.0800000000000000)*(x35)))+(((0.0800000000000000)*(x33)))))))+(((0.0740000000000000)*(x12)))+(((x5)*(((((0.0800000000000000)*(x32)))+(((-0.0800000000000000)*(x31))))))));
eerot[6]=((((-1.00000000000000)*(x47)*(x5)))+(((x16)*(x50)*(x6))));
eerot[7]=((((x13)*(x19)*(((((-1.00000000000000)*(x48)))+(x49)))))+(((x25)*(((((-1.00000000000000)*(x16)*(x47)))+(((-1.00000000000000)*(x5)*(x50)*(x6))))))));
eerot[8]=((((x19)*(((((-1.00000000000000)*(x16)*(x47)))+(((-1.00000000000000)*(x5)*(x50)*(x6)))))))+(((x13)*(x25)*(x50))));
eetrans[2]=((0.390950000000000)+(((x5)*(((((-0.0800000000000000)*(x46)))+(((-0.0800000000000000)*(x45)))))))+(((x16)*(x6)*(((((0.0800000000000000)*(x48)))+(((-0.0800000000000000)*(x49)))))))+(((-0.295000000000000)*(x46)))+(((-0.295000000000000)*(x45)))+(((0.0900000000000000)*(x49)))+(((-0.0900000000000000)*(x48)))+(((0.270000000000000)*(x1))));
}

IKFAST_API int getNumFreeParameters() { return 0; }
IKFAST_API int* getFreeParameters() { return NULL; }
IKFAST_API int getNumJoints() { return 6; }

IKFAST_API int getIKRealSize() { return sizeof(IKReal); }

IKFAST_API int getIKType() { return 0x67000001; }

class IKSolver {
public:
IKReal j0,cj0,sj0,htj0,j1,cj1,sj1,htj1,j2,cj2,sj2,htj2,j3,cj3,sj3,htj3,j4,cj4,sj4,htj4,j5,cj5,sj5,htj5,new_r00,r00,rxp0_0,new_r01,r01,rxp0_1,new_r02,r02,rxp0_2,new_r10,r10,rxp1_0,new_r11,r11,rxp1_1,new_r12,r12,rxp1_2,new_r20,r20,rxp2_0,new_r21,r21,rxp2_1,new_r22,r22,rxp2_2,new_px,px,npx,new_py,py,npy,new_pz,pz,npz,pp;

bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
for(int dummyiter = 0; dummyiter < 1; ++dummyiter) {
vsolutions.resize(0); vsolutions.reserve(8);
r00 = eerot[0*3+0];
r01 = eerot[0*3+1];
r02 = eerot[0*3+2];
r10 = eerot[1*3+0];
r11 = eerot[1*3+1];
r12 = eerot[1*3+2];
r20 = eerot[2*3+0];
r21 = eerot[2*3+1];
r22 = eerot[2*3+2];
px = eetrans[0]; py = eetrans[1]; pz = eetrans[2];

new_r00=((-1.00000000000000)*(r02));
new_r01=r01;
new_r02=r00;
new_px=((((-0.0800000000000000)*(r00)))+(px));
new_r10=((-1.00000000000000)*(r12));
new_r11=r11;
new_r12=r10;
new_py=((((-0.0800000000000000)*(r10)))+(py));
new_r20=((-1.00000000000000)*(r22));
new_r21=r21;
new_r22=r20;
new_pz=((-0.390950000000000)+(((-0.0800000000000000)*(r20)))+(pz));
r00 = new_r00; r01 = new_r01; r02 = new_r02; r10 = new_r10; r11 = new_r11; r12 = new_r12; r20 = new_r20; r21 = new_r21; r22 = new_r22; px = new_px; py = new_py; pz = new_pz;
pp=(((px)*(px))+((py)*(py))+((pz)*(pz)));
npx=((((px)*(r00)))+(((py)*(r10)))+(((pz)*(r20))));
npy=((((px)*(r01)))+(((py)*(r11)))+(((pz)*(r21))));
npz=((((px)*(r02)))+(((py)*(r12)))+(((pz)*(r22))));
rxp0_0=((((-1.00000000000000)*(py)*(r20)))+(((pz)*(r10))));
rxp0_1=((((px)*(r20)))+(((-1.00000000000000)*(pz)*(r00))));
rxp0_2=((((-1.00000000000000)*(px)*(r10)))+(((py)*(r00))));
rxp1_0=((((-1.00000000000000)*(py)*(r21)))+(((pz)*(r11))));
rxp1_1=((((px)*(r21)))+(((-1.00000000000000)*(pz)*(r01))));
rxp1_2=((((-1.00000000000000)*(px)*(r11)))+(((py)*(r01))));
rxp2_0=((((-1.00000000000000)*(py)*(r22)))+(((pz)*(r12))));
rxp2_1=((((px)*(r22)))+(((-1.00000000000000)*(pz)*(r02))));
rxp2_2=((((-1.00000000000000)*(px)*(r12)))+(((py)*(r02))));
{
IKReal j0array[2], cj0array[2], sj0array[2];
bool j0valid[2]={false};
IKReal x51=((-1.00000000000000)*(py));
IKReal x52=IKatan2(x51, px);
j0array[0]=((-1.00000000000000)*(x52));
sj0array[0]=IKsin(j0array[0]);
cj0array[0]=IKcos(j0array[0]);
j0array[1]=((3.14159265358979)+(((-1.00000000000000)*(x52))));
sj0array[1]=IKsin(j0array[1]);
cj0array[1]=IKcos(j0array[1]);
if( j0array[0] > IKPI )
{
    j0array[0]-=IK2PI;
}
else if( j0array[0] < -IKPI )
{    j0array[0]+=IK2PI;
}
j0valid[0] = true;
if( j0array[1] > IKPI )
{
    j0array[1]-=IK2PI;
}
else if( j0array[1] < -IKPI )
{    j0array[1]+=IK2PI;
}
j0valid[1] = true;
if( j0valid[0] && j0valid[1] && IKabs(cj0array[0]-cj0array[1]) < 0.0001 && IKabs(sj0array[0]-sj0array[1]) < 0.0001 )
{
    j0valid[1]=false;
}
for(int ij0 = 0; ij0 < 2; ++ij0)
{
if( !j0valid[ij0] )
{
    continue;
}
j0 = j0array[ij0]; cj0 = cj0array[ij0]; sj0 = sj0array[ij0];

{
IKReal j2array[2], cj2array[2], sj2array[2];
bool j2valid[2]={false};
IKReal x53=((0.888629283483560)*(py)*(sj0));
IKReal x54=((0.888629283483560)*(cj0)*(px));
IKReal x55=((0.975985144601143)+(x54)+(x53));
IKReal x56=((6.00425191542946)*(pp));
IKReal x57=((x55)+(((-1.00000000000000)*(x56))));
if( (x57) < -1-IKFAST_SINCOS_THRESH || (x57) > 1+IKFAST_SINCOS_THRESH )
    continue;
IKReal x58=IKasin(x57);
j2array[0]=((-2.84547750771483)+(((-1.00000000000000)*(x58))));
sj2array[0]=IKsin(j2array[0]);
cj2array[0]=IKcos(j2array[0]);
j2array[1]=((0.296115145874965)+(x58));
sj2array[1]=IKsin(j2array[1]);
cj2array[1]=IKcos(j2array[1]);
if( j2array[0] > IKPI )
{
    j2array[0]-=IK2PI;
}
else if( j2array[0] < -IKPI )
{    j2array[0]+=IK2PI;
}
j2valid[0] = true;
if( j2array[1] > IKPI )
{
    j2array[1]-=IK2PI;
}
else if( j2array[1] < -IKPI )
{    j2array[1]+=IK2PI;
}
j2valid[1] = true;
if( j2valid[0] && j2valid[1] && IKabs(cj2array[0]-cj2array[1]) < 0.0001 && IKabs(sj2array[0]-sj2array[1]) < 0.0001 )
{
    j2valid[1]=false;
}
for(int ij2 = 0; ij2 < 2; ++ij2)
{
if( !j2valid[ij2] )
{
    continue;
}
j2 = j2array[ij2]; cj2 = cj2array[ij2]; sj2 = sj2array[ij2];

{
IKReal dummyeval[1];
IKReal gconst0;
gconst0=IKsign(((0.0199800000000000)+(((-0.270000000000000)*(py)*(sj0)))+(((-0.0900000000000000)*(pz)*(sj2)))+(((-0.0900000000000000)*(cj2)*(py)*(sj0)))+(((-0.270000000000000)*(cj0)*(px)))+(((0.295000000000000)*(py)*(sj0)*(sj2)))+(((-0.0218300000000000)*(sj2)))+(((-0.295000000000000)*(cj2)*(pz)))+(((-0.0900000000000000)*(cj0)*(cj2)*(px)))+(((0.00666000000000000)*(cj2)))+(((0.295000000000000)*(cj0)*(px)*(sj2)))));
dummyeval[0]=((3.00000000000000)+(((-13.5135135135135)*(pz)*(sj2)))+(cj2)+(((-3.27777777777778)*(sj2)))+(((-13.5135135135135)*(cj2)*(py)*(sj0)))+(((-40.5405405405405)*(py)*(sj0)))+(((-13.5135135135135)*(cj0)*(cj2)*(px)))+(((-44.2942942942943)*(cj2)*(pz)))+(((44.2942942942943)*(py)*(sj0)*(sj2)))+(((44.2942942942943)*(cj0)*(px)*(sj2)))+(((-40.5405405405405)*(cj0)*(px))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j1array[1], cj1array[1], sj1array[1];
bool j1valid[1]={false};
IKReal x59=(sj2)*(sj2);
IKReal x60=(cj2)*(cj2);
j1array[0]=IKatan2(((-1.00000000000000)*(gconst0)*(((0.0729000000000000)+(((-1.00000000000000)*((pz)*(pz))))+(((-0.159300000000000)*(sj2)))+(((0.0486000000000000)*(cj2)))+(((-0.0531000000000000)*(cj2)*(sj2)))+(((0.00810000000000000)*(x60)))+(((0.0870250000000000)*(x59)))))), ((gconst0)*(((((-0.0243000000000000)*(sj2)))+(((0.0789250000000000)*(cj2)*(sj2)))+(((0.0740000000000000)*(pz)))+(((-1.00000000000000)*(cj0)*(px)*(pz)))+(((-0.0265500000000000)*(x60)))+(((-0.0796500000000000)*(cj2)))+(((-1.00000000000000)*(py)*(pz)*(sj0)))+(((0.0265500000000000)*(x59)))))));
sj1array[0]=IKsin(j1array[0]);
cj1array[0]=IKcos(j1array[0]);
if( j1array[0] > IKPI )
{
    j1array[0]-=IK2PI;
}
else if( j1array[0] < -IKPI )
{    j1array[0]+=IK2PI;
}
j1valid[0] = true;
for(int ij1 = 0; ij1 < 1; ++ij1)
{
if( !j1valid[ij1] )
{
    continue;
}
j1 = j1array[ij1]; cj1 = cj1array[ij1]; sj1 = sj1array[ij1];

rotationfunction0(vsolutions);
}
}

}

}
}
}
}
}
}
return vsolutions.size()>0;
}
inline void rotationfunction0(std::vector<IKSolution>& vsolutions) {
for(int rotationiter = 0; rotationiter < 1; ++rotationiter) {
IKReal x61=((cj2)*(sj1));
IKReal x62=((cj1)*(sj2));
IKReal x63=((x61)+(x62));
IKReal x64=((-1.00000000000000)*(x63));
IKReal x65=((sj1)*(sj2));
IKReal x66=((cj1)*(cj2));
IKReal x67=((x65)+(((-1.00000000000000)*(x66))));
IKReal x68=((x66)+(((-1.00000000000000)*(x65))));
new_r00=((((r20)*(x67)))+(((cj0)*(r00)*(x64)))+(((r10)*(sj0)*(x64))));
new_r01=((((r21)*(x67)))+(((cj0)*(r01)*(x64)))+(((r11)*(sj0)*(x64))));
new_r02=((((r12)*(sj0)*(x64)))+(((r22)*(x67)))+(((cj0)*(r02)*(x64))));
new_r10=((((-1.00000000000000)*(r00)*(sj0)))+(((cj0)*(r10))));
new_r11=((((-1.00000000000000)*(r01)*(sj0)))+(((cj0)*(r11))));
new_r12=((((cj0)*(r12)))+(((-1.00000000000000)*(r02)*(sj0))));
new_r20=((((r20)*(x64)))+(((cj0)*(r00)*(x68)))+(((r10)*(sj0)*(x68))));
new_r21=((((r21)*(x64)))+(((cj0)*(r01)*(x68)))+(((r11)*(sj0)*(x68))));
new_r22=((((r12)*(sj0)*(x68)))+(((r22)*(x64)))+(((cj0)*(r02)*(x68))));
{
IKReal j4array[2], cj4array[2], sj4array[2];
bool j4valid[2]={false};
cj4array[0]=new_r22;
if( cj4array[0] >= -1-IKFAST_SINCOS_THRESH && cj4array[0] <= 1+IKFAST_SINCOS_THRESH )
{
    j4valid[0] = j4valid[1] = true;
    j4array[0] = IKacos(cj4array[0]);
    sj4array[0] = IKsin(j4array[0]);
    cj4array[1] = cj4array[0];
    j4array[1] = -j4array[0];
    sj4array[1] = -sj4array[0];
}
else if( isnan(cj4array[0]) )
{
    // probably any value will work
    j4valid[0] = true;
    cj4array[0] = 1; sj4array[0] = 0; j4array[0] = 0;
}
if( j4valid[0] && j4valid[1] && IKabs(cj4array[0]-cj4array[1]) < 0.0001 && IKabs(sj4array[0]-sj4array[1]) < 0.0001 )
{
    j4valid[1]=false;
}
for(int ij4 = 0; ij4 < 2; ++ij4)
{
if( !j4valid[ij4] )
{
    continue;
}
j4 = j4array[ij4]; cj4 = cj4array[ij4]; sj4 = sj4array[ij4];

{
IKReal dummyeval[1];
IKReal gconst2;
gconst2=IKsign(sj4);
dummyeval[0]=sj4;
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal dummyeval[1];
IKReal gconst1;
gconst1=IKsign((((new_r12)*(new_r12))+((new_r02)*(new_r02))));
dummyeval[0]=(((new_r12)*(new_r12))+((new_r02)*(new_r02)));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal evalcond[7];
IKReal x69=((-1.00000000000000)+(new_r22));
evalcond[0]=((-3.14159265358979)+(IKfmod(((3.14159265358979)+(j4)), 6.28318530717959)));
evalcond[1]=x69;
evalcond[2]=new_r20;
evalcond[3]=new_r21;
evalcond[4]=new_r20;
evalcond[5]=new_r21;
evalcond[6]=x69;
if( IKabs(evalcond[0]) < 0.0000010000000000  && IKabs(evalcond[1]) < 0.0000010000000000  && IKabs(evalcond[2]) < 0.0000010000000000  && IKabs(evalcond[3]) < 0.0000010000000000  && IKabs(evalcond[4]) < 0.0000010000000000  && IKabs(evalcond[5]) < 0.0000010000000000  && IKabs(evalcond[6]) < 0.0000010000000000  )
{
{
IKReal j3array[2], cj3array[2], sj3array[2];
bool j3valid[2]={false};
IKReal x70=IKatan2(new_r02, new_r12);
j3array[0]=((-1.00000000000000)*(x70));
sj3array[0]=IKsin(j3array[0]);
cj3array[0]=IKcos(j3array[0]);
j3array[1]=((3.14159265358979)+(((-1.00000000000000)*(x70))));
sj3array[1]=IKsin(j3array[1]);
cj3array[1]=IKcos(j3array[1]);
if( j3array[0] > IKPI )
{
    j3array[0]-=IK2PI;
}
else if( j3array[0] < -IKPI )
{    j3array[0]+=IK2PI;
}
j3valid[0] = true;
if( j3array[1] > IKPI )
{
    j3array[1]-=IK2PI;
}
else if( j3array[1] < -IKPI )
{    j3array[1]+=IK2PI;
}
j3valid[1] = true;
if( j3valid[0] && j3valid[1] && IKabs(cj3array[0]-cj3array[1]) < 0.0001 && IKabs(sj3array[0]-sj3array[1]) < 0.0001 )
{
    j3valid[1]=false;
}
for(int ij3 = 0; ij3 < 2; ++ij3)
{
if( !j3valid[ij3] )
{
    continue;
}
j3 = j3array[ij3]; cj3 = cj3array[ij3]; sj3 = sj3array[ij3];

{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((((-1.00000000000000)*(cj3)*(new_r01)))+(((-1.00000000000000)*(new_r11)*(sj3)))), ((((new_r10)*(sj3)))+(((cj3)*(new_r00)))));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}
}
}

} else
{
evalcond[0]=((-3.14159265358979)+(IKfmod(((1.11022302462516e-16)+(j4)), 6.28318530717959)));
evalcond[1]=((1.00000000000000)+(new_r22));
evalcond[2]=new_r20;
evalcond[3]=new_r21;
evalcond[4]=((-1.00000000000000)*(new_r20));
evalcond[5]=((-1.00000000000000)*(new_r21));
evalcond[6]=((-1.00000000000000)+(((-1.00000000000000)*(new_r22))));
if( IKabs(evalcond[0]) < 0.0000010000000000  && IKabs(evalcond[1]) < 0.0000010000000000  && IKabs(evalcond[2]) < 0.0000010000000000  && IKabs(evalcond[3]) < 0.0000010000000000  && IKabs(evalcond[4]) < 0.0000010000000000  && IKabs(evalcond[5]) < 0.0000010000000000  && IKabs(evalcond[6]) < 0.0000010000000000  )
{
{
IKReal j3array[2], cj3array[2], sj3array[2];
bool j3valid[2]={false};
IKReal x71=IKatan2(new_r02, new_r12);
j3array[0]=((-1.00000000000000)*(x71));
sj3array[0]=IKsin(j3array[0]);
cj3array[0]=IKcos(j3array[0]);
j3array[1]=((3.14159265358979)+(((-1.00000000000000)*(x71))));
sj3array[1]=IKsin(j3array[1]);
cj3array[1]=IKcos(j3array[1]);
if( j3array[0] > IKPI )
{
    j3array[0]-=IK2PI;
}
else if( j3array[0] < -IKPI )
{    j3array[0]+=IK2PI;
}
j3valid[0] = true;
if( j3array[1] > IKPI )
{
    j3array[1]-=IK2PI;
}
else if( j3array[1] < -IKPI )
{    j3array[1]+=IK2PI;
}
j3valid[1] = true;
if( j3valid[0] && j3valid[1] && IKabs(cj3array[0]-cj3array[1]) < 0.0001 && IKabs(sj3array[0]-sj3array[1]) < 0.0001 )
{
    j3valid[1]=false;
}
for(int ij3 = 0; ij3 < 2; ++ij3)
{
if( !j3valid[ij3] )
{
    continue;
}
j3 = j3array[ij3]; cj3 = cj3array[ij3]; sj3 = sj3array[ij3];

{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((((new_r11)*(sj3)))+(((cj3)*(new_r01)))), ((((-1.00000000000000)*(cj3)*(new_r00)))+(((-1.00000000000000)*(new_r10)*(sj3)))));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}
}
}

} else
{
if( 1 )
{
continue;

} else
{
}
}
}
}

} else
{
{
IKReal j3array[1], cj3array[1], sj3array[1];
bool j3valid[1]={false};
j3array[0]=IKatan2(((gconst1)*(new_r12)*(sj4)), ((gconst1)*(new_r02)*(sj4)));
sj3array[0]=IKsin(j3array[0]);
cj3array[0]=IKcos(j3array[0]);
if( j3array[0] > IKPI )
{
    j3array[0]-=IK2PI;
}
else if( j3array[0] < -IKPI )
{    j3array[0]+=IK2PI;
}
j3valid[0] = true;
for(int ij3 = 0; ij3 < 1; ++ij3)
{
if( !j3valid[ij3] )
{
    continue;
}
j3 = j3array[ij3]; cj3 = cj3array[ij3]; sj3 = sj3array[ij3];

{
IKReal dummyeval[1];
IKReal gconst3;
gconst3=IKsign(sj4);
dummyeval[0]=sj4;
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IKReal evalcond[11];
IKReal x72=((cj3)*(new_r12));
IKReal x73=((new_r02)*(sj3));
IKReal x74=((((-1.00000000000000)*(x73)))+(x72));
IKReal x75=((-1.00000000000000)+(new_r22));
IKReal x76=((new_r12)*(sj3));
IKReal x77=((cj3)*(new_r02));
IKReal x78=((x77)+(x76));
evalcond[0]=((-3.14159265358979)+(IKfmod(((3.14159265358979)+(j4)), 6.28318530717959)));
evalcond[1]=x75;
evalcond[2]=new_r20;
evalcond[3]=new_r21;
evalcond[4]=x74;
evalcond[5]=x74;
evalcond[6]=x78;
evalcond[7]=new_r20;
evalcond[8]=new_r21;
evalcond[9]=x75;
evalcond[10]=x78;
if( IKabs(evalcond[0]) < 0.0000010000000000  && IKabs(evalcond[1]) < 0.0000010000000000  && IKabs(evalcond[2]) < 0.0000010000000000  && IKabs(evalcond[3]) < 0.0000010000000000  && IKabs(evalcond[4]) < 0.0000010000000000  && IKabs(evalcond[5]) < 0.0000010000000000  && IKabs(evalcond[6]) < 0.0000010000000000  && IKabs(evalcond[7]) < 0.0000010000000000  && IKabs(evalcond[8]) < 0.0000010000000000  && IKabs(evalcond[9]) < 0.0000010000000000  && IKabs(evalcond[10]) < 0.0000010000000000  )
{
{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((((-1.00000000000000)*(cj3)*(new_r01)))+(((-1.00000000000000)*(new_r11)*(sj3)))), ((((new_r10)*(sj3)))+(((cj3)*(new_r00)))));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}

} else
{
IKReal x79=((cj3)*(new_r12));
IKReal x80=((new_r02)*(sj3));
IKReal x81=((x79)+(((-1.00000000000000)*(x80))));
IKReal x82=((new_r12)*(sj3));
IKReal x83=((cj3)*(new_r02));
IKReal x84=((x82)+(x83));
evalcond[0]=((-3.14159265358979)+(IKfmod(((1.11022302462516e-16)+(j4)), 6.28318530717959)));
evalcond[1]=((1.00000000000000)+(new_r22));
evalcond[2]=new_r20;
evalcond[3]=new_r21;
evalcond[4]=x81;
evalcond[5]=x81;
evalcond[6]=x84;
evalcond[7]=((-1.00000000000000)*(new_r20));
evalcond[8]=((-1.00000000000000)*(new_r21));
evalcond[9]=((-1.00000000000000)+(((-1.00000000000000)*(new_r22))));
evalcond[10]=((-1.00000000000000)*(x84));
if( IKabs(evalcond[0]) < 0.0000010000000000  && IKabs(evalcond[1]) < 0.0000010000000000  && IKabs(evalcond[2]) < 0.0000010000000000  && IKabs(evalcond[3]) < 0.0000010000000000  && IKabs(evalcond[4]) < 0.0000010000000000  && IKabs(evalcond[5]) < 0.0000010000000000  && IKabs(evalcond[6]) < 0.0000010000000000  && IKabs(evalcond[7]) < 0.0000010000000000  && IKabs(evalcond[8]) < 0.0000010000000000  && IKabs(evalcond[9]) < 0.0000010000000000  && IKabs(evalcond[10]) < 0.0000010000000000  )
{
{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((((new_r11)*(sj3)))+(((cj3)*(new_r01)))), ((((-1.00000000000000)*(cj3)*(new_r00)))+(((-1.00000000000000)*(new_r10)*(sj3)))));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}

} else
{
if( 1 )
{
continue;

} else
{
}
}
}
}

} else
{
{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((gconst3)*(new_r21)), ((-1.00000000000000)*(gconst3)*(new_r20)));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}

}

}
}
}

}

}

} else
{
{
IKReal j5array[1], cj5array[1], sj5array[1];
bool j5valid[1]={false};
j5array[0]=IKatan2(((gconst2)*(new_r21)), ((-1.00000000000000)*(gconst2)*(new_r20)));
sj5array[0]=IKsin(j5array[0]);
cj5array[0]=IKcos(j5array[0]);
if( j5array[0] > IKPI )
{
    j5array[0]-=IK2PI;
}
else if( j5array[0] < -IKPI )
{    j5array[0]+=IK2PI;
}
j5valid[0] = true;
for(int ij5 = 0; ij5 < 1; ++ij5)
{
if( !j5valid[ij5] )
{
    continue;
}
j5 = j5array[ij5]; cj5 = cj5array[ij5]; sj5 = sj5array[ij5];

{
IKReal dummyeval[1];
IKReal gconst4;
gconst4=IKsign((((new_r12)*(new_r12))+((new_r02)*(new_r02))));
dummyeval[0]=(((new_r12)*(new_r12))+((new_r02)*(new_r02)));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IKReal j3array[1], cj3array[1], sj3array[1];
bool j3valid[1]={false};
j3array[0]=IKatan2(((gconst4)*(new_r12)*(sj4)), ((gconst4)*(new_r02)*(sj4)));
sj3array[0]=IKsin(j3array[0]);
cj3array[0]=IKcos(j3array[0]);
if( j3array[0] > IKPI )
{
    j3array[0]-=IK2PI;
}
else if( j3array[0] < -IKPI )
{    j3array[0]+=IK2PI;
}
j3valid[0] = true;
for(int ij3 = 0; ij3 < 1; ++ij3)
{
if( !j3valid[ij3] )
{
    continue;
}
j3 = j3array[ij3]; cj3 = cj3array[ij3]; sj3 = sj3array[ij3];

{
vsolutions.push_back(IKSolution()); IKSolution& solution = vsolutions.back();
solution.basesol.resize(6);
solution.basesol[0].foffset = j0;
solution.basesol[1].foffset = j1;
solution.basesol[2].foffset = j2;
solution.basesol[3].foffset = j3;
solution.basesol[4].foffset = j4;
solution.basesol[5].foffset = j5;
solution.vfree.resize(0);
}
}
}

}

}
}
}

}

}
}
}
}
}};


/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API bool ik(const IKReal* eetrans, const IKReal* eerot, const IKReal* pfree, std::vector<IKSolution>& vsolutions) {
IKSolver solver;
return solver.ik(eetrans,eerot,pfree,vsolutions);
}

IKFAST_API const char* getKinematicsHash() { return "204673ec97731ed369d9c210d57f3890"; }

#ifdef IKFAST_NAMESPACE
} // end namespace
#endif

#ifndef IKFAST_NO_MAIN
#include <stdio.h>
#include <stdlib.h>
#ifdef IKFAST_NAMESPACE
using namespace IKFAST_NAMESPACE;
#endif
int main(int argc, char** argv)
{
    if( argc != 12+getNumFreeParameters()+1 ) {
        printf("\nUsage: ./ik r00 r01 r02 t0 r10 r11 r12 t1 r20 r21 r22 t2 free0 ...\n\n"
               "Returns the ik solutions given the transformation of the end effector specified by\n"
               "a 3x3 rotation R (rXX), and a 3x1 translation (tX).\n"
               "There are %d free parameters that have to be specified.\n\n",getNumFreeParameters());
        return 1;
    }

    std::vector<IKSolution> vsolutions;
    std::vector<IKReal> vfree(getNumFreeParameters());
    IKReal eerot[9],eetrans[3];
    eerot[0] = atof(argv[1]); eerot[1] = atof(argv[2]); eerot[2] = atof(argv[3]); eetrans[0] = atof(argv[4]);
    eerot[3] = atof(argv[5]); eerot[4] = atof(argv[6]); eerot[5] = atof(argv[7]); eetrans[1] = atof(argv[8]);
    eerot[6] = atof(argv[9]); eerot[7] = atof(argv[10]); eerot[8] = atof(argv[11]); eetrans[2] = atof(argv[12]);
    for(std::size_t i = 0; i < vfree.size(); ++i)
        vfree[i] = atof(argv[13+i]);
    bool bSuccess = ik(eetrans, eerot, vfree.size() > 0 ? &vfree[0] : NULL, vsolutions);

    if( !bSuccess ) {
        fprintf(stderr,"Failed to get ik solution\n");
        return -1;
    }

    printf("Found %d ik solutions:\n", (int)vsolutions.size());
    std::vector<IKReal> sol(getNumJoints());
    for(std::size_t i = 0; i < vsolutions.size(); ++i) {
        printf("sol%d (free=%d): ", (int)i, (int)vsolutions[i].GetFree().size());
        std::vector<IKReal> vsolfree(vsolutions[i].GetFree().size());
        vsolutions[i].GetSolution(&sol[0],vsolfree.size()>0?&vsolfree[0]:NULL);
        for( std::size_t j = 0; j < sol.size(); ++j)
            printf("%.15f, ", sol[j]);
        printf("\n");
    }
    return 0;
}

#endif
