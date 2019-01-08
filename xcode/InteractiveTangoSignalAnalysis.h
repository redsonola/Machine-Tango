//
//  InteractiveTangoSignalAnalysis.h
//  InteractiveTangoReadFromAndroid
//
//  Created by courtney on 10/12/14.
//
//

#ifndef InteractiveTangoReadFromAndroid_InteractiveTangoSignalAnalysis_h
#define InteractiveTangoReadFromAndroid_InteractiveTangoSignalAnalysis_h

//#include "ext.h"
#include <complex>
#include <math.h>
#include <algorithm>
#include "fftw3.h"

//#include "itbase.h"
//#include "itsignal.h"

#include "itpp/itbase.h"
#include "itpp/itsignal.h"
#include "itpp/itstat.h"


# define BUFFER_SIZE 4096
# define MAX_OUT 2049 //must be BUFFER SIZE/2+1

#define USE_ACCEL 0
#define USE_GYRO 1
#define USE_MAG 2

const int SAMPLE_SZ = 4096;

namespace InteractiveTango {
    
    static const double SR = 51.2; // Sample rate
    
class UGEN
{
public:
    enum PlacesToSend {Ableton=0, Wekinator=1};
    UGEN(){};
    virtual std::vector<ci::osc::Message> getOSC()=0;
    virtual void update(float seconds=0)= 0;
//    virtual PlacesToSend placeToSend() { return PlacesToSend::Ableton; };
    
};

class SignalAnalysis : public UGEN
{
protected:
    std::vector<ShimmerData *> data1;
    std::vector<ShimmerData *> data2;
    
    SignalAnalysis *ugen, *ugen2  ;
    int buffersize;
    
    bool useAccel;
    bool useGry;
    bool useQuart;
    
    virtual double findAvg(std::vector<double> input, int start, int end)
    {
        double N = end - start;
        double sum = 0;
        for(int i=start; i<end; i++)
        {
            sum += input[i];
        }
        return (sum/N);
    };
    
public:
	SignalAnalysis(SignalAnalysis *s1 = NULL, int bufsize=1024, SignalAnalysis *s2 = NULL)
	{
        ugen = s1;
        ugen2 = s2;
        setBufferSize(bufsize);
        
        useAccel=true;
        useGry=false;
        useQuart=false;
        
//OMFG CHANGE THIS
        
//        if(s1!=NULL) s1->setBufferSize(buffersize);
//        if(s2!=NULL) s2->setBufferSize(buffersize);
	};
    
//    enum SignalAnalysisType
//    {
//        Generic,
//        InputSignal,
//        Filter,
//        StepDetection,
//        SpatialSimilarity,
//        WindowedVariance,
//        WindowedVarianceDerivative
//    };
    
    inline virtual int getNewSampleCount()
    {
        if(ugen != NULL && ugen2==NULL)
            return ugen->getNewSampleCount();
        else if(ugen != NULL && ugen2!=NULL)
            return std::max(ugen->getNewSampleCount(), ugen2->getNewSampleCount());
        else return 0;
    }
    
    inline void processAccel(bool a)
    {
        useAccel = a;
    };
    
    inline void processGry(bool g)
    {
        useGry = g;
    };
    
    inline void processQuart(bool q)
    {
        useQuart=q;
    };
    
    inline virtual void setBufferSize(int sz)
    {
        buffersize = sz;
    };
    
    virtual inline std::vector<ShimmerData *> getBuffer(){
        return data1;
    };
    
    virtual inline std::vector<ShimmerData *> getBuffer2(){
        return data2;
    };
    
    int getBufferSize(){return buffersize;};
    
	virtual void update(float seconds=0)
    {
        if( ugen != NULL ) data1 = ugen->getBuffer();
        if( ugen2 != NULL ) data2 = ugen2->getBuffer();
    };
    
    std::vector<itpp::vec> toITPPVector(std::vector<ShimmerData *> sdata)
    {
        std::vector<itpp::vec> newVec;
        for(int i=0; i<20; i++)
        {
            itpp::vec v(sdata.size());
            for(int j=0; j<sdata.size(); j++)
            {
                v.set( j, sdata[j]->getData(i));
            }
            newVec.push_back(v);
        }
        return newVec;
    };
    std::vector<float> toFloatVector(itpp::vec input)
    {
        std::vector<float> output;
        for(int i=0; i<input.length(); i++)
        {
            output.push_back(input[i]);
        }
        return output;
    };
    
    virtual double findAvg(std::vector<ShimmerData *> input, int start, int end, int index)
    {
        double N = end - start;
        double sum = 0;
        for(int i=start; i<end; i++)
        {
            sum += input[i]->getData(index);
        }
        return (sum/N);
    };
};

    
//has output signals that it modifies  & forwards on to others
//only handles one stream of data... don't have any for two at the moment...
class OutputSignalAnalysis : public SignalAnalysis
{
protected:
    std::vector<ShimmerData *> outdata1;
    std::vector<ShimmerData *> outdata2;
    
    void eraseData()
    {
        for( int i=0; i<outdata1.size(); i++ )
            delete outdata1[i];
        
        for( int i=0; i<outdata2.size(); i++ )
            delete outdata2[i];
        
        outdata1.clear();
        outdata2.clear();
    };
public:
    
    OutputSignalAnalysis(SignalAnalysis *s1, int bufsize, SignalAnalysis *s2 = NULL) : SignalAnalysis(s1, bufsize, s2)
    {
    
    }

    //puts in accel data slots -- all other data left alone -- ALSO only
    void toShimmerOutputVector( std::vector<float> inputX, std::vector<float> inputY, std::vector<float> inputZ )
    {
        for(int i=0; i<inputX.size(); i++)
        {
            ShimmerData *data = new ShimmerData();
            data->setData(0, data1[i]->getData(0));
            data->setData(1, data1[i]->getData(1));
            data->setData(2, inputX[i]);
            data->setData(3, inputY[i]);
            data->setData(4, inputZ[i]);
            outdata1.push_back(data);
        }
    };
    
    float curOrLastValue(int index, std::vector<float> v)
    {
        if(index >= v.size()) return v[v.size()-1];
        else return v[index];
    };
    
    //puts in gyro AND accel data slots -- all other data left alone -- ALSO only
    void toShimmerOutputVector(std::vector<float> inputX, std::vector<float> inputY, std::vector<float> inputZ, std::vector<float> gX, std::vector<float> gY, std::vector<float> gZ,
                               std::vector<float> qX, std::vector<float> qY, std::vector<float> qZ, std::vector<float> qA)
    {
        //accounting for no data
        std::vector<std::vector<float>> vectors = { inputX, inputY, inputZ, gX, gY, gZ, qX, qY, qZ, qA };
        int maxSize=0;//, maxIndex=0;
        
        for(int i=0; i<vectors.size(); i++)
        {
            int curSize = vectors[i].size();
            if(curSize > maxSize)
            {
                maxSize = curSize;
//                maxIndex=i;
            }
        }
        
        for(int i=0; i<maxSize; i++){
            ShimmerData *data = new ShimmerData();
            data->setData(0, data1[i]->getData(0));
            data->setData(1, data1[i]->getData(1));
            outdata1.push_back(data);
        }
        
        if( useAccel )
        {
            for(int i=0; i<maxSize; i++){
                outdata1[i]->setData(2, curOrLastValue(i, inputX));
                outdata1[i]->setData(3, curOrLastValue(i, inputY));
                outdata1[i]->setData(4, curOrLastValue(i, inputZ));
            }
        }
        
        if( useGry)
        {
            for(int i=0; i<maxSize; i++){
                outdata1[i]->setData(11, curOrLastValue(i, gX));
                outdata1[i]->setData(12, curOrLastValue(i, gY));
                outdata1[i]->setData(13, curOrLastValue(i, gZ));
            }
        }
        
        if( useQuart )
        {
            for(int i=0; i<maxSize; i++)
            {
                outdata1[i]->setQuarternion(curOrLastValue(i, qX), curOrLastValue(i, qY), curOrLastValue(i, qZ), curOrLastValue(i, qA) );
            }
        }

    };
    
    virtual void update(float seconds = 0){
        eraseData();
        SignalAnalysis::update(seconds);
    };
    
    virtual std::vector<ShimmerData *> getBuffer(){
        return outdata1;
    };
    
    virtual std::vector<ShimmerData *> getBuffer2(){
        return outdata2;
    };
};
    
    
//only gets inputs so sends nothing -- not an output bc it doesn't modify its inputs
    //TODO: this filters for acc data ONLY, so turn on options, etc.
class InputSignal : public SignalAnalysis
{
protected:
//    ShimmerData * lastValue;  //ok, TODO: kill when done
    int ID1;
    bool isAndroid;
public:
    InputSignal(int idz, bool android, SignalAnalysis *s1 = NULL, int bufsize=512, SignalAnalysis *s2 = NULL) : SignalAnalysis(s1, bufsize, s2)
    {
//        lastValue = NULL;
        ID1= idz;
        isAndroid = android;
    };

    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;
        
        return msgs;
    };
    SensorData *sensor  ;

    void setInput( SensorData *s1 )
    {
        sensor = s1;
    };
    
    virtual int getNewSampleCount()
    {
        return sensor->getNewSampleCount();
    };
    
	virtual void update(float seconds=0)  //TODO... maybe just change bufsize & check when filtering.
    {
        data1.clear(); 
        if( sensor != NULL )
        {
            std::vector<ShimmerData *> data = sensor->getBuffer(buffersize);
            for(int i =0; i<data.size(); i++) //FILTERING OUT UNWANTED DATA>>> NOTE!!!
            {
                if( data[i]->getData(ShimmerData::DataIndices::ACCELX) != NO_DATA )
                {
                    data1.push_back(data[i]);
//                    std::cout << data[i]->getQuarternion(0) << "," << data[i]->getQuarternion(1) << "," << data[i]->getQuarternion(2) << ","  << data[i]->getQuarternion(3) << std::endl;
                }
            }
        }
    };
    
    virtual std::vector<ShimmerData *> getBuffer(){
        return data1;
    };

};
    
    
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class MotionDataOutput
{
protected:
        std::vector<MotionAnalysisData *> motionData;
public:
    MotionDataOutput()
    {};
        
    virtual std::vector<MotionAnalysisData * > getMotionData()
    {
        return motionData;
    };
        
    virtual ~MotionDataOutput()
    {
        for(int i=0; i<motionData.size(); i++)
        {
            if(motionData[i] != NULL)
            {
                delete motionData[i];
            }
        }
    };
        
    MotionAnalysisData *getMotionDataItem(int index)
    {
        assert( index < motionData.size() );
        return motionData[index];
    };
    virtual size_t motionDataSize(){ return motionData.size();  };
    virtual void updateMotionData(){}; //overloaded with indices other places, so not abstract
    virtual void setMotionAnalysisMinMax(int i, double minv,  double maxv)
    {
        assert( i<motionData.size() && i>=0);
        ((MotionAnalysisEvent * ) motionData[i])->setMinMax(minv, maxv);
    };
        
};
//-------------------------------------------------------------------------------------------------
class Filter : public OutputSignalAnalysis, public MotionDataOutput
{
protected:
    itpp::vec b;
    itpp::vec a;
    bool useMotionData;
    
public:
    Filter(SignalAnalysis *s1, std::string  _b, std::string  _a, int sz=512 ) : OutputSignalAnalysis(s1, sz)
    {
        b = _b.c_str();
        a = _a.c_str();
        
        double maxGMag = std::sqrt(G_FORCE*2*G_FORCE*2*3);
        std::string names[8] = {"accel x", "accel y", "accel z", "accel mag", "gry x", "gry y", "gry z", "gry mag"};
        for(int i=0; i<8; i++)
        {
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 0));
            if(i!=4 && i!=7)
                motionData[i]->setMinMax(-G_FORCE*2, G_FORCE*2); //gravity
            else motionData[i]->setMinMax(0, maxGMag); //gravity
            motionData[i]->setName(names[i]);
        }
        
        useMotionData = false;
    };
    
    void calcMotionData(bool calc = true){ useMotionData=calc; };
    
    void CoreDoubleN(double *X, int MX, int NX, double *a, double *b,
                     int order, double *Z, double *Y)
    {
        // Direct form II transposed method for general filter length.
        // Implemented as time domain difference equations.
        // INPUT:
        //   X:  Double array. Operation happens of 1st dimension.
        //   MX: Number of elements in the 1st dimension
        //   NX: Number of columns, considers mutliple dimensions.
        //   a, b: Double vector of filter parameters. Both have nParam elements.
        //       The first element of a is 1.
        //   Z:  DOUBLE array, initial conditions.
        //   order: Number of filter parameters, order of filter + 1.
        // OUTPUT:
        //   Z:  DOUBLE array, final conditions.
        //   Y:  Double array, allocated by the caller.
        
        double Xi, Yi;
        int i, j, R;
        
        i = 0;
        while (NX--) {                         // Next slice
            R = i + MX;                         // End of the column
            while (i < R) {
                Xi = X[i];                       // Get signal
                Yi = b[0] * Xi + Z[0];           // Filtered value
                for (j = 1; j < order; j++) {    // Update conditions
                    Z[j - 1] = b[j] * Xi + Z[j] - a[j] * Yi;
                }
                Z[order - 1] = b[order] * Xi - a[order] * Yi;
                
                Y[i++] = Yi;                      // Write to output 
            } 
            Z += order;                          // Next condition vector 
        }
    }
    
    std::vector<float> filterFunc(std::vector<float> input)
    {
        
        double Y[ input.size() ], X[ input.size() ];
        int asize = a.size();
        int bsize = b.size();
        int inputSize = 0;

        double a1[asize], b1[bsize];
        double order = b.size()-1;
        
        for(int i=0; i<input.size(); i++)
        {
            if( X[i] != NO_DATA )
            {
                X[inputSize] = input[i];
                inputSize++;
            }
        }
        
        for(int i=0; i<b.size(); i++){
            b1[i] = b[i];
        }
        
        for(int i=0; i<a.size(); i++)
            a1[i] = a[i];

//        
//        CoreDoubleN( X,  MX,  NX,  a1,  b1, order,  Z,  &Y[0]);
        filter(order, a1, b1, inputSize, X, &Y[0]);
        
        std::vector<float> output;
        for(int i=0; i<inputSize; i++)
        {
            output.push_back(float(Y[i]));
        }
        
        return output;

    };
    
    std::vector<double> filterFunc(std::vector<double> input)
    {
        int MX = input.size();
        int input_size = 0; //after take out no_data
        double Y[ MX ], X[ MX ];
        double a1[a.size()], b1[b.size()];
        double order = b.size()-1;
        
//        std::cout << MX << "\n";
        
        for(int i=0; i<input.size(); i++)
        {
            if(input[i]!=NO_DATA)
            {
                X[input_size] = input[i];
                input_size++;
            }
        }
        
        for(int i=0; i<b.size(); i++){
            b1[i] = b[i];
        }
        
        for(int i=0; i<a.size(); i++)
            a1[i] = a[i];
        
        filter(order, a1, b1, input_size, X, Y);
        
        std::vector<double> output;
        for(int i=0; i<input_size; i++)
        {
            output.push_back((Y[i]));
//            std::cout << i << "," << Y[i] << "\n";

        }
        
//        std::cout << "Y: " << sizeof(Y)/sizeof(Y[0]) << std::endl; 
        assert(input_size == sizeof(Y)/sizeof(Y[0]));
        
        return output;
        
    };

    std::vector<float> shimmerToFloat(int index)
    {
        std::vector<float> output;
        for(int i=0; i < data1.size() ; i++)
        {
            output.push_back( data1[i]->getData(index) );
        }
        return output;
    };
    
    void filter(int ord, double *a, double *b, int np, double *x, double *y)
    {
        int i,j;
        y[0]=b[0]*x[0];
        for (i=1;i<ord+1;i++)
        {
            y[i]=0.0;
            for (j=0;j<i+1;j++)
                y[i]=y[i]+b[j]*x[i-j];
            for (j=0;j<i;j++)
                y[i]=y[i]-a[j+1]*y[i-j-1];
        }
        /* end of initial part */
        for (i=ord+1;i<np+1;i++)
        {
            y[i]=0.0;
            for (j=0;j<ord+1;j++)
                y[i]=y[i]+b[j]*x[i-j];
            for (j=0;j<ord;j++)
                y[i]=y[i]-a[j+1]*y[i-j-1];
        }
        return;
    } /* end of filter */
    
    void updateMotionData()
    {
        if(getNewSampleCount() < 1) return ;
        if( outdata1.size() < 1) return;
        
        for( int i=2; i<5; i++ )
        {
//            std::cout << " i: " << i << " outdata1.size(): " << outdata1.size() << " start: " << outdata1.size() - getNewSampleCount() <<std::endl;
//            if( outdata1.size() - getNewSampleCount() > outdata1.size() )
//            {
//                std::cout << "f'd up sensor data\n";
//                motionData[i-2]->setValue(findAvg(outdata1, std::max(int(outdata1.size() - getNewSampleCount()), 0), outdata1.size(), i));
//                std::abort();
//            }
            
            motionData[i-2]->setValue(findAvg(outdata1, std::max(int(outdata1.size() - getNewSampleCount()), 0), outdata1.size(), i));
        }
        //accel mag
        motionData[3]->setValue( std::sqrt(motionData[0]->asDouble()*motionData[0]->asDouble() + motionData[1]->asDouble()*motionData[1]->asDouble() + motionData[2]->asDouble()*motionData[2]->asDouble()) );
        
        //gyros
        for( int i=11; i<14; i++ )
            motionData[i-7]->setValue(findAvg(outdata1, std::max(int(outdata1.size() - getNewSampleCount()), 0), outdata1.size(), i));
        
        //gyro mag
        motionData[7]->setValue( std::sqrt(motionData[4]->asDouble()*motionData[4]->asDouble() + motionData[5]->asDouble()*motionData[5]->asDouble() + motionData[6]->asDouble()*motionData[6]->asDouble()) );
    };
    
    //TODO: oh god will have to clean this up
    std::vector<double> update(std::vector<double> inSig)
    {
        return filterFunc( inSig );
    };

    //only so far on accel values
    virtual void update(float seconds=0)
    {
        OutputSignalAnalysis::update(seconds);
        if( data1.size() < b.size() + 5 ) return ;
        
        std::vector<float> resX, resY, resZ;
        if( useAccel )
        {       //this only does accel
            resX = filterFunc( shimmerToFloat(2) );
            resY = filterFunc( shimmerToFloat(3) );
            resZ = filterFunc( shimmerToFloat(4) );
        }
        
        std::vector<float> gX, gY, gZ;
        if( useGry )
        {//now, also do gyro
            gX = filterFunc( shimmerToFloat(11) );
            gY = filterFunc( shimmerToFloat(12) );
            gZ = filterFunc( shimmerToFloat(13) );
        }
        
        std::vector<float> qX, qY, qZ, qA;
        if( useQuart  )
        { //now, also do quarternions
            qX = filterFunc( shimmerToFloat(20) );
            qY = filterFunc( shimmerToFloat(21) );
            qZ = filterFunc( shimmerToFloat(22) );
            qA = filterFunc( shimmerToFloat(23) );
        }
        
        toShimmerOutputVector( resX, resY, resZ, gX, gY, gZ, qX, qY, qZ, qA ); //always puts results in accel place...
        
        
        if(useMotionData) updateMotionData();
        
        
//        std::vector<itpp::vec> sdata1 = toITPPVector( data1 );
//
//            itpp::vec resX = itpp::filter( b, a, sdata1[2] );
//            itpp::vec resY = itpp::filter( b, a, sdata1[3] );
//            itpp::vec resZ = itpp::filter( b, a, sdata1[4] );
//
//            toShimmerOutputVector( toFloatVector(resX), toFloatVector(resY), toFloatVector(resZ) ); //always puts results in accel place...
    };
    
    //dummy for OSC for now.
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;
        return msgs;
    };
    
};
    
class LPFilter5Hz : public Filter
{
public:
    // low-pass filter is 6 degrees, 5Hz - cut-off
    //co-efficients from matlab, from butterworth
    LPFilter5Hz(SignalAnalysis *s1, int sz=512) : Filter(s1, "0.2413 0.2413", "1.0000 -0.5173", sz)
    {
    };
    
};
    
    class HPFilter1xHz : public Filter
    {
    public:
        // low-pass filter is 6 degrees, 5Hz - cut-off
        //co-efficients from matlab, from butterworth
        HPFilter1xHz(SignalAnalysis *s1, int sz=512) : Filter(s1, "0.9999 -0.9999", "1.0000 -0.9999", sz)
        {
        };
        
    };
    
    class LPFilter1Hz2d : public Filter
    {
    public:
        // low-pass filter is 6 degrees, 5Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter1Hz2d(SignalAnalysis *s1, int sz=512) : Filter(s1, "0.00001213 0.00004854 0.00007281 0.00004854 0.00001213", "1.0000 -3.6794 5.0886 -3.1345 0.7255", sz)
        {
        };
        
    };
    
    class LPFilter8Hz : public Filter
    {
    public:
        // low-pass filter is 3 degrees, 8Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter8Hz(SignalAnalysis *s1, int sz=512) : Filter(s1,"0.0552 0.1656 0.1656 0.0552", "1.0000 -1.0810 0.6465 -0.1240", sz)
        {
        };
        
    };
    

    class LPFilter5Hz5d : public Filter
    {
    public:
        //co-efficients from matlab, from butterworth
        LPFilter5Hz5d(SignalAnalysis *s1, int sz=512) : Filter(s1,"0.0012 0.0058 0.0116 0.0116 0.0058 0.0012", "1.0000 -3.0225 3.9098 -2.6389 0.9207 -0.1319", sz)
        {
            
        };
        
    };
    
    class LPFilter10Hz : public Filter
    {
    public:
        // low-pass filter is 3 degrees, 10Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter10Hz(SignalAnalysis *s1, int sz=512) : Filter(s1, "0.0940 0.2821 0.2821 0.0940", "1.0000 -0.6227 0.4363 -0.0614", sz)
        {
        };
        
    };
    
    class LPFilter20Hz : public Filter
    {
    public:
        // low-pass filter is 3 degrees, 10Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter20Hz(SignalAnalysis *s1, int sz=512) : Filter(s1,
        "0.1640 1.3120 4.5920 9.1841 11.4801 9.1841 4.5920 1.3120 0.1640",
        "1.0000 4.4841 9.2881 11.4399 9.0940 4.7529 1.5889 0.3097 0.0269", sz)
        {
            
            //  b= 0.1640 1.3120 4.5920 9.1841 11.4801 9.1841 4.5920 1.3120 0.1640
            
            //  a= 1.0000    4.4841    9.2881   11.4399    9.0940    4.7529 1.5889    0.3097    0.0269
        
        };
        
    };
    
    class LPFilter15Hz : public Filter
    {
    public:
        // low-pass filter is 1 degrees, 15Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter15Hz(SignalAnalysis *s1, int sz=512) : Filter(s1,"0.5697 0.5697","1.0000 0.1395", sz)
        {
        };
        
    };
    
    class LPFilter1Hz : public Filter
    {
    public:
        // low-pass filter is 3 degrees, 1Hz - cut-off
        //co-efficients from matlab, from butterworth
        LPFilter1Hz(SignalAnalysis *s1, int sz=3060) : Filter(s1,
        "0.0002051 0.0006153 0.0006153 0.0002051",
        "1.0000 -2.7547 2.5386 -0.7822", sz)
        {
        };
    };



//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
    
    
//------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class AveragingFilter : public OutputSignalAnalysis, public MotionDataOutput
{
protected:
    int windowSize;
public:
    
    
    AveragingFilter(SignalAnalysis *s1, int w=10, int bufsize=512 ) : OutputSignalAnalysis(s1, bufsize)
    {
        windowSize = w;
    };
    
    //I'm gonna be shot for yet another avg function
    float shimmerAvg(std::vector<ShimmerData *> data, int start, int end, int index )
    {
        double sum = 0;
        int valCount = 0;
        for( int j=start; j<=end; j++ )
        {
            if(data1[j]->getData(index) != NO_DATA)
            {
                sum += data1[j]->getData(index);
                valCount++;
            }
        }
        if(valCount==0) return NO_DATA;
            else return sum / double( valCount );
    };
    
    virtual void update(float seconds=0)
    {
        OutputSignalAnalysis::update(seconds);
        if( data1.size() < buffersize) return ;
        
        for( int i=0; i<data1.size(); i++ )
        {
            int start = std::max(0, i-windowSize);
            int end = i;
            ShimmerData *sd = new ShimmerData();
            sd->setData(ShimmerData::DataIndices::INDEX, data1[i]->getData(ShimmerData::DataIndices::INDEX));
            sd->setData(ShimmerData::DataIndices::TIME_STAMP, data1[i]->getData(ShimmerData::DataIndices::TIME_STAMP));
            
            if( useAccel )
            {
                sd->setData(ShimmerData::DataIndices::ACCELX, shimmerAvg(data1, start, end, ShimmerData::DataIndices::ACCELX));
                sd->setData(ShimmerData::DataIndices::ACCELY, shimmerAvg(data1, start, end, ShimmerData::DataIndices::ACCELY));
                sd->setData(ShimmerData::DataIndices::ACCELZ, shimmerAvg(data1, start, end, ShimmerData::DataIndices::ACCELZ));
            }
            
            if( useGry )
            {
                sd->setData(ShimmerData::DataIndices::GYROX, shimmerAvg(data1, start, end, ShimmerData::DataIndices::GYROX));
                sd->setData(ShimmerData::DataIndices::GYROY, shimmerAvg(data1, start, end, ShimmerData::DataIndices::GYROY));
                sd->setData(ShimmerData::DataIndices::GYROZ, shimmerAvg(data1, start, end, ShimmerData::DataIndices::GYROZ));
            }
            
            if( useQuart )
            {
                sd->setData(ShimmerData::DataIndices::QX, shimmerAvg(data1, start, end, ShimmerData::DataIndices::QX));
                sd->setData(ShimmerData::DataIndices::QY, shimmerAvg(data1, start, end, ShimmerData::DataIndices::QY));
                sd->setData(ShimmerData::DataIndices::QZ, shimmerAvg(data1, start, end, ShimmerData::DataIndices::QZ));
                sd->setData(ShimmerData::DataIndices::QA, shimmerAvg(data1, start, end, ShimmerData::DataIndices::QA));
            }
            outdata1.push_back(sd);
        }
    }
};
    
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


//Signal Analysis objects that have outcomes which interface with mapping schemeas, etc. etc. -- need to srsly refactor this, as similar to other things, sharing data, but do NOT have time NOW
class SignalAnalysisEventOutput : public SignalAnalysis, public MotionDataOutput
{
public:
    SignalAnalysisEventOutput(SignalAnalysis *s1 = NULL, int bufsize=1024, SignalAnalysis *s2 = NULL)  : SignalAnalysis(s1, bufsize, s2)
    {
    };

};

    
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

    
class WindowedVariance : public SignalAnalysisEventOutput
{
protected:
    double curVariance;
    int ID1;
    std::string port;
    
    //3-23-2016 -- created for gesture recog
    double curVarXYZ[3];


public:
    WindowedVariance(SignalAnalysis *s1, int sz=SR/3, int idz=0, std::string p="" ) : SignalAnalysisEventOutput(s1, sz)
    {
        curVariance = 0;
        ID1 = idz;
        port = p;
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 0));
        
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 1)); //x
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 2)); //y
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 3)); //z

        motionData[0]->setName("Windowed Variance");
        motionData[1]->setName("Windowed Variance X");
        motionData[2]->setName("Windowed Variance Y");
        motionData[3]->setName("Windowed Variance Z");

        setMotionAnalysisMinMax(0, 0, 11);
        
        for(int i=0; i<3; i++)
            curVarXYZ[i]=0;
    };
    
    double getVariance()
    {
        return curVariance;
    };
    
    double getVariance(int i)
    {
        assert(i>=0 && i<3);
        return curVarXYZ[i];
    };
    
    virtual void updateMotionData()
    {
       ( (MotionAnalysisEvent *)motionData[0])->setValue(curVariance);
        for (int i=0; i<3; i++)
        {
            ( (MotionAnalysisEvent *)motionData[i+1])->setValue(curVarXYZ[i]);
        }
    };
    
    virtual void updateXYZ(std::vector<ShimmerData *> buf)
    {
        for(int i=0; i<3; i++)
        {
            double mean = findAvg(buf, 0, buffersize, i+2);
            double sum = 0;
            for(int j=0; j<buffersize; j++)
            {
                double diff = (buf[j]->getData(i+2) - mean);
                sum = sum + (diff*diff);
            }
        
            curVarXYZ[i] = sum / buffersize;
            curVarXYZ[i] = curVarXYZ[i]  / (G_FORCE * 2.5);
        }
    }
    
    virtual void update(float seconds=0)
    {
        SignalAnalysis::update(seconds);
        if( data1.size() < buffersize) return ;
        
        std::vector<ShimmerData *>::iterator startIter = data1.end() - buffersize;
        std::vector<ShimmerData *>::iterator endIter = data1.end();
        std::vector<ShimmerData *> buf( startIter, endIter );
        
        std::vector<double>  mag;
        for(int i=0; i<buffersize; i++)
        {
            mag.push_back( std::sqrt(buf[i]->getData(2)*buf[i]->getData(2) + buf[i]->getData(3)*buf[i]->getData(3) + buf[i]->getData(4)*buf[i]->getData(4)) );
//            std::cout << "mag: " << mag[i]/(G_FORCE * 2) << "," <<  buf[i]->getData(2) << "," << buf[i]->getData(3) << "," << buf[i]->getData(4) << std::endl;
        }
        
        double mean = SignalAnalysis::findAvg(mag, 0, buffersize);
        double sum = 0;
        for(int i=0; i<buffersize; i++)
        {
            double diff = (mag[i] - mean);
            sum = sum + (diff*diff);
        }
        
        curVariance = sum / buffersize;
        curVariance = curVariance / (G_FORCE * 2.5);
        
        updateXYZ(buf);
        
        updateMotionData();
        
        
        
//        int snapSample = data1.size() - std::min(getNewSampleCount(), buffersize);
//        for(int i=snapSample; i<data1.size(); i++)
//        {
//            std::cout << data1[i]->getData(0) << " curVar , " << curVariance << "\n";
//        }

    };
    
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;
        ci::osc::Message msg;

//disabling OSC messages that we are not using 4-8-2015
//        msg.addIntArg(ID1);
//        msg.addIntArg(atoi(port.c_str()));
//        msg.setAddress(SHIMMER_WINDOWED_VARIATION);
//        msg.addFloatArg(curVariance);
//        
//        msgs.push_back(msg);
        
        return msgs;
    };

};
//------------------------------------------
//------------------------------------------
     

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //the CHANGE in variance
    
    class WindowedVarianceDifference : public SignalAnalysisEventOutput
    {
    protected:
        double curVarianceDiff, curVarianceDiffAvg, lastVar;
        WindowedVariance *variance;
        int ID1;
        std::string port;
        std::vector<double> pastVar;
        std::vector<double> pastVarDiff;
        
        std::vector<double> pastVarXYZ[3];
        std::vector<double> pastVarDiffXYZ[3];
        double curVarianceDiffXYZ[3], curVarianceDiffAvgXYZ[3], lastVarXYZ[3];
        
        int window;

    public:
        enum MotionDataIndices {AVG=0, X=1, Y=2, Z=3 };
        WindowedVarianceDifference(WindowedVariance *v, int idz, std::string p ) : SignalAnalysisEventOutput(NULL, 0)
        {
            
            lastVar = 0;
            ID1 = idz;
            port = p;
            variance = v;
            
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 0));
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 1)); //x
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 2)); //y
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 3)); //z

            
            motionData[0]->setName("Windowed Variance Diff");
            motionData[1]->setName("Windowed Variance Diff X");
            motionData[2]->setName("Windowed Variance Diff Y");
            motionData[3]->setName("Windowed Variance Diff Z");

            
            curVarianceDiffAvg = 0;
            window = variance->getBufferSize() * 2 ; //roughly a second buffer
            
            //THIS HAS BEEN SUPERCEDED BY MotionAnalysisParameterRanges!
            //TODO: Check this AGAIN and make this come from a DATABASE FILE! ALL OF THEM!
//            float maxVarDifference = 0.00508361;
//            setMotionAnalysisMinMax(0, 0, maxVarDifference);
            
            //init the x y z vals
            for(int i=0; i<3; i++)
            {
                lastVarXYZ[i] = 0;
                curVarianceDiffAvgXYZ[i]=0;
            }
            
        };
        
        virtual void setWindowSize(int sz)
        {
            window = sz;
        };

        virtual void updateMotionData()
        {
            ((MotionAnalysisEvent *)motionData[0])->setValue(curVarianceDiffAvg);
            
            for(int i=0; i<3; i++)
            {
                ((MotionAnalysisEvent *)motionData[i+1])->setValue(curVarianceDiffAvgXYZ[i]);
            }
        };
        
        double getVarianceDiff()
        {
            return curVarianceDiff;
        };
        
        double getVarianceDiff(int i)
        {
            return curVarianceDiffXYZ[i];
        };
        
        void updateXYZ()
        {
            for(int k=0; k<3; k++)
            {
                pastVarXYZ[k].push_back( variance->getVariance(k) );
                if ( pastVarXYZ[k].size() < variance->getBufferSize()  ) return;
            
                if( pastVarXYZ[k].size() >  variance->getBufferSize() )
                {
                    pastVarXYZ[k].erase(pastVarXYZ[k].begin());
                }
            
                if( pastVarDiffXYZ[k].size() >  ( window ) )
                {
                    pastVarDiffXYZ[k].erase(pastVarDiffXYZ[k].begin());
                }
            
                LPFilter10Hz lpfilter(NULL);
                std::vector<double> lpVar = lpfilter.update(pastVarXYZ[k]);
                curVarianceDiffXYZ[k] = std::abs( pastVarXYZ[k].at( pastVarXYZ[k].size()-1 ) - pastVarXYZ[k].at( pastVarXYZ[k].size()-2 ) );
                pastVarDiffXYZ[k].push_back(curVarianceDiffXYZ[k]);
            
                curVarianceDiffAvgXYZ[k] = 0;
                if( pastVarDiffXYZ[k].size()>0 )
                {
                    for(int i=0; i<pastVarDiffXYZ[k].size(); i++)
                    {
                        curVarianceDiffAvgXYZ[k] += pastVarDiffXYZ[k].at(i);
                    }
                    curVarianceDiffAvgXYZ[k] /= pastVarDiffXYZ[k].size();
                }
            }
        };
        
        virtual void update(float seconds=0)
        {
//          curVarianceDiff = variance->getVariance() - lastVar;
//          lastVar = variance->getVariance() ;
            
            pastVar.push_back( variance->getVariance() );
            if ( pastVar.size() < variance->getBufferSize()  ) return;
            
            if( pastVar.size() >  variance->getBufferSize() )
            {
                pastVar.erase(pastVar.begin());
            }
            
            if( pastVarDiff.size() >  ( window ) )
            {
                pastVarDiff.erase(pastVarDiff.begin());
            }
            
            LPFilter10Hz lpfilter(NULL);
            std::vector<double> lpVar = lpfilter.update(pastVar);
            curVarianceDiff = std::abs( pastVar[ pastVar.size()-1 ] - pastVar[ pastVar.size()-2 ] );
            pastVarDiff.push_back(curVarianceDiff);
            
            curVarianceDiffAvg = 0;
            if( pastVarDiff.size()>0 )
            {
                for(int i=0; i<pastVarDiff.size(); i++)
                {
                    curVarianceDiffAvg += pastVarDiff[i];
                }
                curVarianceDiffAvg /= pastVarDiff.size();
            }
            updateMotionData();
//            std::cout << "curVARDIFF: " << curVarianceDiffAvg << std::endl;
        };
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            ci::osc::Message msg;
            
            
            //disabling OSC messages that we are not using 4-8-2015
            
//            msg.addIntArg(ID1);
//            msg.addIntArg(atoi(port.c_str()));
//            msg.setAddress(SHIMMER_WINDOWED_VARIATION_DIFF);
//            msg.addFloatArg(curVarianceDiff);
//            
//            msgs.push_back(msg);
            
            return msgs;
        };
        
    };
    
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //essentially the first derivative of the input signal, but in portions of  x y z
    //results in the shimmer data, like a filter or input -- not a motion data thing
    
    class SignalVector : public OutputSignalAnalysis, public MotionDataOutput
    {
    protected:
        int ID1;
        ci::Vec3d prevXYZ;
        ci::Vec3d prevGyroXYZ;
        ci::Vec4d prevQuat;
        
        //TODO: fix.....
        ci::Vec3d min_g, max_g;
        
        //this gives one answer per buffer
        std::vector<ShimmerData *> window;
        ShimmerData *curSignalVector;
        int windowSize;
//        CircularIndex windowIndex;
//        double curValue; //move to motion data output form if needed, for now, plateaus at this one value
        
    public: //just changed from 15 to 10 for windowsize
        SignalVector(SignalAnalysis *s, int idz, int sz=25 ) : OutputSignalAnalysis(s, sz)
        {
            
            ID1 = idz;
            windowSize = sz;
            for(int i=0; i<3; i++ )
            {
                prevXYZ[i] = 0;
                prevGyroXYZ[i] = 0;
                prevQuat[i]=0;
            }
            prevQuat[3]=0;
            
//            windowIndex.setMinMax(0, sz-1);
            curSignalVector=NULL;
            
            motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 0));
            motionData[0]->setName("Gyro Signal Vector - Magnitude");
            motionData[0]->setMinMax(0, 1);
            
            //in process -- to be used
            min_g = {-399.46300,  -291.65700,    -1.49210};
            max_g = {425.29300,   259.82100,     1.60732};
            
        };
        
        ci::Vec3d getOutputGyroAvg()
        {
            ci::Vec3d sum;
            for( int i=0; i<outdata1.size(); i++ )
            {
                sum += outdata1[i]->getGyro();
            }
            return sum / double( outdata1.size() );
            
            motionData[0]->setValue( sum.length() );
            
        }
        
        virtual double scale(double val, int i)
        {
            double denom = max_g[i] - min_g[i];
            if ( denom == 0 ) return 0;
            else return ( val - min_g[i] ) / denom;
        }
        
        virtual void updateMotionData()
        {
            ci::Vec3d avg = getOutputGyroAvg();
            ci::Vec3d data;
            for( int i=0; i<3; i++ )
            {
                data[i] = scale(avg[i], i);
            }
            
            
            
        }

        
        ci::Vec3d getUnitVector(ci::Vec3d curvec, ci::Vec3d prevVec)
        {
        //see if should normalize vector when using windowing
            ci::Vec3d unitVec = curvec - prevVec ;
//            unitVec.normalize() ;
            return unitVec ;
        };
        
        
        ci::Vec4d getUnitVector4d(ci::Vec4d curvec, ci::Vec4d prevVec)
        {
            //see if should normalize vector when using windowing
            ci::Vec4d unitVec = curvec - prevVec ;
//            unitVec.normalize() ;
            return unitVec ;
        };
        
        virtual void clearWindow()
        {
            for( int i=0; i<window.size(); i++ )
            {
                if( window[i] != NULL ) delete window[i];
            }
            window.clear();
        };
        
        //for this it is average --
        virtual ShimmerData getWindowAvg()
        {
            ShimmerData sum;
            int accelCount=0;
            int gyroCount=0;
            int quartCount=0;
            for( int i=0; i<window.size(); i++ )
            {
                if(useAccel)
                    if( window[i]->getAccelData().x != NO_DATA)
                    {
                        accelCount++;
                        sum.setAccelData( window[i]->getAccelData() + sum.getAccelData() );
                    }
                
                if( useGry )
                    if( window[i]->getGyro().x != NO_DATA)
                    {
                        gyroCount++;
                        sum.setGyro( window[i]->getGyro() + sum.getGyro() );
                    }
                
                if(useQuart)
                    if( window[i]->getQuarternion(0) != NO_DATA)
                    {
                        quartCount++;
                        sum.setQuarternion( window[i]->getQuarternionVec4d() + sum.getQuarternionVec4d() );
                    }
            }
            if(useAccel) sum.setAccelData( sum.getAccelData() / double(accelCount) );
            if (useGry)  sum.setGyro( sum.getGyro() / double(gyroCount) );
            if (useQuart) sum.setQuarternion( sum.getQuarternionVec4d() / double(quartCount) );
            
            return sum;
        };
        
        
        
        virtual void update(float seconds=0)
        {
            OutputSignalAnalysis::update(seconds);
            ci::Vec3d curXYZ, curGyro;
            ci::Vec4d curQuat;
            if( data1.size() < 1 ) return ;
            
            for(int i=0; i<data1.size(); i++)
            {

                //add to data window
                ShimmerData *sd = new ShimmerData();
                sd->setAccelData( data1[i]->getAccelData() );
                sd->setGyro( data1[i]->getGyro() );
                sd->setQuarternion(data1[i]->getQuarternionVec4d());
                window.push_back(sd);
                
                
                //update the vector if enough values in window
                if( window.size() >= windowSize )
                {
                    ShimmerData d = getWindowAvg();
                    curXYZ  = d.getAccelData();
                    curGyro = d.getGyro();
                    curQuat = d.getQuarternionVec4d();
                    
                    if( curSignalVector != NULL )
                    {
                        delete curSignalVector;
                    }
                    curSignalVector = new ShimmerData();
                    curSignalVector->setAccelData( getUnitVector( curXYZ, prevXYZ ) );
                    curSignalVector->setGyro( getUnitVector( curGyro,  prevGyroXYZ ) );
                    curSignalVector->setQuarternion( getUnitVector4d( curQuat,  prevQuat ) );
                    
//                    std::cout << "new vector: " << curSignalVector->getAccelData().x << "," << curSignalVector->getAccelData().y << "," << curSignalVector->getAccelData().z << "\n";
                    
                    prevXYZ  = curXYZ;
                    prevGyroXYZ = curGyro;
                    prevQuat = curQuat;
                    
                    clearWindow();
                }
                
                //add cur vector to output buffer
                if( curSignalVector != NULL )
                {
                    ShimmerData *s = new ShimmerData();
                    s->setAccelData( curSignalVector->getAccelData() );
                    s->setGyro( curSignalVector->getGyro() );
                    s->setQuarternion(curSignalVector->getQuarternionVec4d());
                    outdata1.push_back(s);
                }
            }
            updateMotionData();

        };
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            ci::osc::Message msg;
            
            //this should not update its own message

            return msgs;
        };
    };
    
    class SignalVectorMinMax : public SignalAnalysisEventOutput
    {
    protected:
        int ID1;

        ci::Vec3d prevXYZ, prevGyroXYZ;
        ci::Vec3d minAccXYZ, minGyroXYZ;
        ci::Vec3d maxAccXYZ, maxGyroXYZ;
        
        ci::Vec3d prevXYZ2, prevGyroXYZ2;
        ci::Vec3d minAccXYZ2, minGyroXYZ2;
        ci::Vec3d maxAccXYZ2, maxGyroXYZ2;
        
        //try with raw signal...
        ci::Vec3d avgAccXYZ1, minAccXYZ1, maxAccXYZ1;
                                //vectors used in motiondata
        std::vector<ci::Vec3d *> motionDataVecs;
        
        std::vector<std::string> nameStrings;
        
        //this gives one answer per buffer
        std::vector<ShimmerData *> window, window2nd, windowPrimary;
//        ShimmerData *curSignalVector;
        int windowSize;
    public:
                                                        //changed from 15 to 10 --5/25/2016
        SignalVectorMinMax(SignalAnalysis *s, int idz, int sz=5 ) : SignalAnalysisEventOutput(s, sz)
        {
            
            ID1 = idz;
            windowSize = sz;
            for(int i=0; i<3; i++ )
            {
                prevXYZ[i] = 0;
                prevGyroXYZ[i] = 0;
            }
            
            nameStrings = {"Min Acc X", "Min Acc Y", "Min Acc Z", "Max Acc X", "Max Acc Y", "Max Acc Z",
                "Min Gyr X", "Min Gyr Y", "Min Gyr Z", "Max Gyr X", "Max Gyr Y", "Max Gyr Z",
                "2 Min Acc X", "2 Min Acc Y", "2 Min Acc Z", "2 Max Acc X", "2 Max Acc Y", "2 Max Acc Z",
                "2 Min Gyr X", " 2Min Gyr Y", "2 Min Gyr Z", "2 Max Gyr X", "2 Max Gyr Y", "2 Max Gyr Z",
                "Min Acc 1st X", "Min Acc 1st Y", "Min Acc 1st Z", "Max Acc 1st X", "Max Acc 1st Y", "Max Acc 1st Z",
                "Avg 1st X", "Avg 2nd Y", "Avg 3rd Z"};
            
            
            motionDataVecs = { &minAccXYZ, &minGyroXYZ,
                &maxAccXYZ, &maxGyroXYZ,
                &minAccXYZ2, &minGyroXYZ2,
                &maxAccXYZ2, &maxGyroXYZ2,
                &avgAccXYZ1, &minAccXYZ1, &maxAccXYZ1 };

            
            for(int i=0; i<nameStrings.size(); i++)
            {
                motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, i)); 
                motionData[i]->setName(nameStrings[i]);
            };
            
            //init max and min
            for(int i=0; i<motionDataVecs.size(); i++ )
            {
                motionDataVecs[i]->set(NO_DATA, NO_DATA, NO_DATA );
            }
            prevXYZ.set(0,0,0);
            prevGyroXYZ.set(0,0,0);
            prevXYZ2.set(0,0,0);
            prevGyroXYZ2.set(0,0,0);
        };
        
        virtual void clearWindow(std::vector<ShimmerData *> &w )
        {
            for( int i=0; i<w.size(); i++ )
            {
                if( w[i] != NULL ) delete w[i];
            }
            w.clear();
        };
        
        virtual void clearWindow()
        {
            clearWindow(window);
            clearWindow(window2nd);
            clearWindow(windowPrimary);
        };
        
        ci::Vec3d findMin(ci::Vec3d m1, ci::Vec3d m2)
        {
            ci::Vec3d m;
            for( int i=0; i<3; i++ )
            {
                if(m1[i] == NO_DATA && m2[i] != NO_DATA )
                    m[i] = m2[i];
                else if(m1[i] != NO_DATA && m2[i] == NO_DATA)
                    m[i] = m1[i];
                else
                    m[i] = std::min(m1[i], m2[i]);
            }
            return m;
        };
        
        ci::Vec3d findMax(ci::Vec3d m1, ci::Vec3d m2)
        {
            ci::Vec3d m;
            for( int i=0; i<3; i++ )
            {
                m[i] = std::max(m1[i], m2[i]);
            }
            return m;
        };
        
        //only for for acc
        virtual ci::Vec3d getAccelWindowAvg( std::vector<ShimmerData *> w )
        {
            ci::Vec3d sum(0,0,0);
            for( int i=0; i<w.size(); i++ )
            {
                sum = w[i]->getAccelData() + sum ;
            }
            sum = sum / double(w.size() );
            return sum;
        };
        
        //find min and max for this TODO: refactor
        virtual void getWindowMinMax()
        {
            for( int j=0; j<window.size(); j++ )
            {
                minAccXYZ = findMin(minAccXYZ, window[j]->getAccelData() );
                minGyroXYZ = findMin(minGyroXYZ, window[j]->getGyro() );
                maxAccXYZ = findMax(maxAccXYZ, window[j]->getAccelData() );
                maxGyroXYZ = findMax(maxGyroXYZ, window[j]->getGyro()  );
                
                minAccXYZ2 = findMin(minAccXYZ2, window2nd[j]->getAccelData() );
                minGyroXYZ2 = findMin(minGyroXYZ2, window2nd[j]->getGyro() );
                maxAccXYZ2 = findMax(maxAccXYZ2, window2nd[j]->getAccelData() );
                maxGyroXYZ2 = findMax(maxGyroXYZ2, window2nd[j]->getGyro()  );
                
                minAccXYZ1 = findMin(minAccXYZ1, windowPrimary[j]->getAccelData() );
                maxAccXYZ1 = findMax(maxAccXYZ1, windowPrimary[j]->getAccelData() );
                avgAccXYZ1 = getAccelWindowAvg(windowPrimary);
            }
        };
        
        virtual void updateMotionData()
        {
            for( int i=0; i<motionDataVecs.size(); i++  )
            {
                for( int j=0; j<3; j++ )
                {
                    ci::Vec3d vd = *motionDataVecs[i];
                    motionData[j+(i*3)]->setValue( vd[j] );
//                    std::cout << vd[j] << ",";
                }
            }
//            std::cout << std::endl;
        };
        
        virtual void addToWindow(std::vector<ShimmerData *> &w, ci::Vec3d curA, ci::Vec3d curG )
        {
            //add to data window
            ShimmerData *sd = new ShimmerData();
            sd->setAccelData(  curA  );
            sd->setGyro(  curG );
            w.push_back(sd);

        };
        
        virtual void addToWindow(std::vector<ShimmerData *> &w, ci::Vec3d prevA, ci::Vec3d prevG, ci::Vec3d curA, ci::Vec3d curG )
        {
            //add to data window
            addToWindow(w, curA - prevA, curG - prevG);
            prevA = curA;
            prevG = curG;
        };
        
        
        virtual void update(float seconds=0)
        {
            SignalAnalysisEventOutput::update(seconds);
            ci::Vec3d curXYZ, curGyro, curXYZ2, curGyro2;
            if( data1.size() < 1 ) return ;
            
            for(int i=0; i<data1.size(); i++)
            {
                
                addToWindow(windowPrimary, data1[i]->getAccelData(), data1[i]->getGyro());
                addToWindow(window, prevXYZ, prevGyroXYZ, data1[i]->getAccelData(), data1[i]->getGyro() );
                
                int index = window.size()-1;
                
                addToWindow(window2nd, prevXYZ2, prevGyroXYZ2, window[index]->getAccelData(), window[index]->getGyro() );
                
                //update the vector if enough values in window
                if( window.size() >= windowSize )
                {
                    
                    //reset min & max
                    //init max and min
                    for(int i=0; i<motionDataVecs.size(); i++ )
                    {
                        motionDataVecs[i]->set( NO_DATA, NO_DATA, NO_DATA );
                    }
                    
                    getWindowMinMax();
                    clearWindow();
                }
                
            }
            
            updateMotionData();

        };
        
        
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            ci::osc::Message msg;
            
            //this should not update its own message
            
            return msgs;
        };


    
    };

    
//------------------------------------------
//------------------------------------------
    //essentially the first derivative of the input signal, but in portions of  x y z
    //results in the shimmer data, like a filter or input -- not a motion data thing
    
    class SignalVector2ndOrder : public SignalVector
    {
    protected:
        ci::Vec3d prevXYZ2;
        ci::Vec3d prevGyroXYZ2;
    public:
        SignalVector2ndOrder(SignalAnalysis *s, int idz, int sz=15 ) : SignalVector(s, sz)
        {
            
            ID1 = idz;
            windowSize = sz;
            for(int i=0; i<3; i++ )
            {
                prevXYZ[i] = 0;
                prevGyroXYZ[i] = 0;
                prevXYZ2[i] = 0;
                prevGyroXYZ2[i] = 0;
            }
            //            windowIndex.setMinMax(0, sz-1);
            curSignalVector=NULL;
            
        };
        
        
        virtual void update(float seconds=0)
        {
            OutputSignalAnalysis::update(seconds);
            ci::Vec3d curXYZ, curGyro;
            ci::Vec3d curXYZ2, curGyro2;

            if( data1.size() < 1 ) return ;
            
            for(int i=0; i<data1.size(); i++)
            {
                //add to data window
                ShimmerData *sd = new ShimmerData();
                curXYZ2 = data1[i]->getAccelData();
                curGyro2 = data1[i]->getGyro();
                
                sd->setAccelData( getUnitVector( curXYZ2, prevXYZ2 ) );
                sd->setGyro( getUnitVector( curGyro2, prevGyroXYZ2 ) );
                window.push_back(sd);
                
                prevXYZ2 = curXYZ2;
                prevGyroXYZ2 = curGyro2;
                
                //update the vector if enough values in window
                if( window.size() >= windowSize )
                {
                    prevXYZ  = curXYZ;
                    prevGyroXYZ = curGyro;
                    
                    ShimmerData d = getWindowAvg();
                    curXYZ  = d.getAccelData();
                    curGyro = d.getGyro();
                    
                    if( curSignalVector != NULL )
                    {
                        delete curSignalVector;
                    }
                    curSignalVector = new ShimmerData();
                    curSignalVector->setAccelData( getUnitVector( curXYZ, prevXYZ) );
                    curSignalVector->setGyro( getUnitVector( curGyro,  prevGyroXYZ) );
                    
                    //                    std::cout << "new vector: " << curSignalVector->getAccelData().x << "," << curSignalVector->getAccelData().y << "," << curSignalVector->getAccelData().z << "\n";
                    
                    clearWindow();
                }
                //add cur vector to output buffer
                if( curSignalVector != NULL )
                {
                    ShimmerData *s = new ShimmerData();
                    s->setAccelData( curSignalVector->getAccelData() );
                    s->setGyro( curSignalVector->getGyro() );
                    outdata1.push_back(s);
                }
            }
        };
    };
    
    //------------------------------------------
    //------------------------------------------
    
    
    
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //essentially the first derivative of the input signal, but in portions of  x y z
    //results in the shimmer data, like a filter or input -- not a motion data thing
    
    //NOT USED NOW -- probably need to do PCA analysis ?????
    
    class HistogramOfDirections : public SignalAnalysisEventOutput
    {
    protected:
        int ID1;
        ci::Vec3d prevXYZ;
        ci::Vec3d prevGyroXYZ;
        
        //this gives one answer per buffer
        std::vector<ShimmerData *> window;
        int windowSize;
        std::vector<std::vector<double>> hod; //the histogram of directions pi to -pi, divided by 8 for ea. x, y, z value
        int histogramSize;
        int histValuesCount;
        
    public:
        HistogramOfDirections(SignalAnalysis *s, int idz, int sz=15, int histSize = 8 ) : SignalAnalysisEventOutput(s, sz)
        {
            
            ID1 = idz;
            windowSize = sz;
            for(int i=0; i<3; i++ )
            {
                prevXYZ[i] = 0;
                prevGyroXYZ[i] = 0;
            }
            
            //init the HOD -- granularity of 8
            histogramSize = histSize;
            histValuesCount = 6; // acc + gry in 3d
            for( int accGryValueIndex = 0;  accGryValueIndex<histValuesCount;  accGryValueIndex++)
            {
                std::vector<double> hodForCurValue;
                for(int i=0; i<histogramSize; i++)
                {
                    hodForCurValue.push_back(0);
                }
                hod.push_back(hodForCurValue);
            }
            
            //TODO: init and update movement data -- is a lot.... probably need to do PCA reduction or only use accel
            for(int i=0; i<histSize*histValuesCount; i++)
                motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, i));
            
        };
        
        ci::Vec3d getDirectionCosines(ci::Vec3d curvec, ci::Vec3d prevVec)
        {
            //see if should normalize vector when using windowing
            ci::Vec3d directionCos;
            ci::Vec3d vec = curvec - prevVec ; //find the vector
            double len = vec.length();
            
            if(len == 0)
            {
                directionCos.x = 0;
                directionCos.y = 0;
                directionCos.z = 0;
            }
            else
            {
                directionCos.x = vec.x / len;
                directionCos.y = vec.y / len;
                directionCos.z = vec.z / len;
            }
            
            return directionCos ;
        };
        
        void updateWindow(int i)
        {
            //add to data window
            ShimmerData *sd = new ShimmerData();
            sd->setAccelData( data1[i]->getAccelData() );
            sd->setGyro( data1[i]->getGyro() );
            window.push_back(sd);
        };
        
        //I KNOW HACKY
        void clearWindow()
        {
            for( int i=0; i<window.size(); i++ )
            {
                if( window[i] != NULL ) delete window[i];
            }
            window.clear();
        };
        
        ShimmerData getWindowAvg()
        {
            ShimmerData sum;
            for( int i=0; i<window.size(); i++ )
            {
                sum.setAccelData( window[i]->getAccelData() + sum.getAccelData() );
                sum.setGyro( window[i]->getGyro() + sum.getGyro() );
            }
            sum.setAccelData( sum.getAccelData() / double(window.size()) );
            sum.setGyro( sum.getGyro() / double(window.size()) );
            return sum;
        };
        
        void placeInHod(std::vector<double> &whichHod, double val)
        {
            //should be from 0..1 -- simply fit into 0-7
            
            int index = round( ((val+1)/2)  * (histogramSize-1) );
            index = std::min(index, histogramSize-1); //umm check maybe assert
            index = std::max(index, 0); //umm check maybe assert

            whichHod[index]++;
        };
        
        void placeInHod(int hodValIndex, double val)
        {
            placeInHod(hod[hodValIndex], val);
        };
        
        void placeInHod(bool isAcc, ci::Vec3d val)
        {
            if(isAcc)
            {
                for(int i=0; i< 3;  i++ )
                    placeInHod(i, val[i]);
            }
            else
            {
                for(int i=3; i< 6;  i++  )
                    placeInHod(i, val[i]);
            }
        };
        
        //place all in correct places
        virtual void updateMotionData()
        {
            for( int accGryValueIndex = 0;  accGryValueIndex<histValuesCount;  accGryValueIndex++)
            {
                for(int i=0; i<histogramSize; i++)
                {
                    motionData[accGryValueIndex*histValuesCount+i]->setValue( (hod.at(accGryValueIndex)).at(i) );
                }
            }
        
        };

        
        virtual void update(float seconds=0)
        {
            SignalAnalysis::update(seconds);
            ci::Vec3d curXYZ, curGyro;
            if( data1.size() < 1 ) return ;
            
            
            for(int i=0; i<data1.size(); i++)
            {
                
                
                updateWindow(i);
                
                //update the vector if enough values in window
                if( window.size() >= windowSize )
                {
                    //clear current hod
                    for( int accGryValueIndex = 0;  accGryValueIndex<histValuesCount;  accGryValueIndex++)
                    {
                        std::vector<double> hodForCurValue;
                        for(int i=0; i<histogramSize; i++)
                        {
                            hod.at(accGryValueIndex)[i] = 0;
                        }
                    }
                    
                    for(int j=0; j<window.size(); j++)
                    {
                        prevXYZ  = curXYZ;
                        prevGyroXYZ = curGyro;
                    
                        ShimmerData *d = window[j];
                        curXYZ  = d->getAccelData();
                        curGyro = d->getGyro();
                        
                        ci::Vec3d acc = getDirectionCosines(curXYZ,prevXYZ);
                        ci::Vec3d gry = getDirectionCosines(curGyro,prevGyroXYZ);
                        
                        placeInHod(true, acc);
                        placeInHod(false, gry);
                        
                    }
                    clearWindow();
                }
                updateMotionData();
            }
        };
        
        virtual std::vector<ci::osc::Message> getOSC()
        {
            std::vector<ci::osc::Message> msgs;
            ci::osc::Message msg;
            
            //this should not update its own message
            
            return msgs;
        };
        
    };
    
    //------------------------------------------
    //------------------------------------------

    
class CrossCoVar : public SignalAnalysisEventOutput
{
protected:
        
    double curPeak, curPeakModVariance;
    int ID1, ID2;
    std::string port1, port2;
    WindowedVariance *variance1, *variance2;
    
        
public:
    CrossCoVar(SignalAnalysis *s1, SignalAnalysis *s2, WindowedVariance *v1, WindowedVariance *v2, int id1, int id2, std::string p1, std::string p2 ) : SignalAnalysisEventOutput(s1, 64, s2)
    {
            
        ID1 = id1;
        ID2 = id2;
        port1 = p1;
        port2 = p2;
        variance1 = v1;
        variance2 = v2;
        motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, 0));
        motionData[0]->setName("CrossCoVar");
        setMotionAnalysisMinMax(0, 0, 0.25);
    };
    
    inline int getID1(){ return ID1; };
    inline int getID2(){ return ID2; };
    
    virtual void updateMotionData()
    {
       ( (MotionAnalysisEvent *)motionData[0])->setValue(curPeakModVariance);
        
    };
    
    virtual void update(float seconds = 0)
    {
        SignalAnalysis::update(seconds);
        if( data1.size()<buffersize || data2.size()<buffersize ) return ;
        
        std::vector<ShimmerData *>::iterator startIter = data1.end() - buffersize;
        std::vector<ShimmerData *>::iterator endIter = data1.end();
        std::vector<ShimmerData *> buf1( startIter, endIter );
        
        std::vector<ShimmerData *>::iterator startIter2 = data2.end() - buffersize;
        std::vector<ShimmerData *>::iterator endIter2 = data2.end();
        std::vector<ShimmerData *> buf2( startIter2, endIter2 );
            
        std::vector<itpp::vec> sdata1 = toITPPVector( buf1 );
        std::vector<itpp::vec> sdata2 = toITPPVector( buf2 );
            
        std::vector<itpp::vec> lpdata1;
        std::vector<itpp::vec> lpdata2;
        
        //mult. by a ones, to make it exactly!!! how matlab does it.
        itpp::vec onesArray(sdata1[1].length()); onesArray.ones();
            
        sdata1[2] = sdata1[2] / (G_FORCE * 2.5);
        sdata1[3] = sdata1[3] / (G_FORCE * 2.5) ;
        sdata1[4] = sdata1[4] / (G_FORCE * 2.5) ;
            
        sdata2[2] = sdata2[2] / (G_FORCE * 2.5);
        sdata2[3] = sdata2[3] / (G_FORCE * 2.5) ;
        sdata2[4] = sdata2[4] / (G_FORCE * 2.5) ;
            
        //to make equivalent to xcov, must subtract the mean
        itpp::vec x1 = sdata1[2] - (itpp::mean(sdata1[2])*onesArray);
        itpp::vec y1 = sdata1[3] - (itpp::mean(sdata1[3])*onesArray);
        itpp::vec z1 = sdata1[4] - (itpp::mean(sdata1[4])*onesArray);
        
        itpp::vec x2 = sdata2[2] - (itpp::mean(sdata2[2])*onesArray);
        itpp::vec y2 = sdata2[3] - (itpp::mean(sdata2[3])*onesArray);
        itpp::vec z2 = sdata2[4] - (itpp::mean(sdata2[4])*onesArray);
        
        //low-pass filter
        //        lpdata1.push_back( itpp::filter( b, a, x1 ) );
        //        lpdata1.push_back( itpp::filter( b, a, y1 ) );
        //        lpdata1.push_back( itpp::filter( b, a, z1 ) );
        //
        //        lpdata2.push_back( itpp::filter( b, a, x2 ) );
        //        lpdata2.push_back( itpp::filter( b, a, y2 ) );
        //        lpdata2.push_back( itpp::filter( b, a, z2 ) );
        //
        
        itpp::vec xCov, yCov, zCov;
        xCov = itpp::xcorr( x1, x2, -1, "unbiased" );
        yCov = itpp::xcorr( y1, y2, -1, "unbiased" );
        zCov = itpp::xcorr( z1, z2, -1, "unbiased" );
        //        xCov = itpp::xcorr( lpdata1[0], lpdata2[0], -1, "coeff" );
        //        yCov = itpp::xcorr( lpdata1[1], lpdata2[1], -1, "coeff" );
        //        zCov = itpp::xcorr( lpdata1[2], lpdata2[2], -1, "coeff" );
        
        // check with means, too
        std::vector<double> magXcov;
        for( int i=0; i< xCov.length(); i++ )
        {
            magXcov.push_back( std::sqrt( xCov.get(i)*xCov.get(i) + yCov.get(i)*yCov.get(i) + zCov.get(i)*zCov.get(i)) );
        }
            
        curPeak = -10000;
        for(int i=0; i<magXcov.size(); i++)
        {
            if( magXcov[i] > curPeak  ) curPeak = magXcov[i];
        }
        
        double meanVar = (variance1->getVariance() + variance2->getVariance())/3.0;
        curPeakModVariance = curPeak / meanVar ;
        updateMotionData();
        
//        int snapSample = buf1.size() - std::min(getNewSampleCount(), buffersize);
//        for(int i=snapSample; i<buf1.size(); i++)
//        {
//                
//            std::cout << sdata1[0].get(i) << " , " << sdata2[0].get(i) <<  " , " << curPeak << " , " << meanVar << " , " << curPeak / meanVar  <<"\n";
////
////                //            std::cout << sdat1v2va1[0].get(i) << " , " << sdata2[0].get(i) << " , "<< x1[0].get(i) << " , "<< y1[1].get(i) << " , "<< z1[2].get(i) << " , "<< x2[0].get(i) << " , "<< y2[1].get(i) << " , "<< z2[2].get(i) <<"\n";
////                
////                //           std::cout << sdata1[0].get(i) << " , " << sdata2[0].get(i) << " , "<< x1.get(i) << " , "<< y1.get(i) << " , "<< z1.get(i) << " , "<< x2.get(i) << " , "<< y2.get(i) << " , "<< z2.get(i) <<"\n";
////                
////                //            std::cout << sdata1[0].get(i) << " , " << sdata2[0].get(i) << " , "<< sdata1[2].get(i) << " , "<< sdata1[3].get(i) << " , "<< sdata1[4].get(i) << " , "<< sdata1[2].get(i) << " , "<< sdata1[3].get(i) << " , "<< sdata1[4].get(i) <<"\n";
////                
//        }
    };
        
    virtual std::vector<ci::osc::Message> getOSC()
    {
        std::vector<ci::osc::Message> msgs;

//disabling OSC messages that we are not using 4-8-2015

        
//        ci::osc::Message msg;
//        msg.addIntArg(ID1);
//        msg.addIntArg(ID2);
//        msg.addIntArg(atoi(port1.c_str()));
//        msg.addIntArg(atoi(port2.c_str()));
//        msg.setAddress(SHIMMER_CROSS_COVARIATION);
//        msg.addFloatArg(curPeak);
//        msg.addFloatArg(curPeakModVariance);

        
//        msgs.push_back(msg);
        
        return msgs;
    };
        
    inline float getPeak()
    {
        return curPeak;
    };
    
};
    
    
    //------------------------------------------
    //------------------------------------------
    
    
    

    
    //average event values across signal analyses,only use for one var things (may modify later)
    //3-1-2015 -- CDB --- Modified so that it averages across multiple motionData, so can use with footsteps, etc.
    class MotionDataAverage : public SignalAnalysisEventOutput
    {
    protected:
        std::vector<SignalAnalysisEventOutput *> signals;
    public:
        MotionDataAverage() : SignalAnalysisEventOutput(NULL, 0, NULL)
        {
            //do nothin -- for the integer version, later
        }
        MotionDataAverage( std::vector<SignalAnalysisEventOutput *> sigz ) : SignalAnalysisEventOutput(NULL, 0, NULL)
        {
            assert(sigz.size() >= 2);
            //also signals should have the same amt of motiondata
            
            signals = sigz;
            
            int N = sigz[0]->getMotionData().size();
            
            for(int i=0; i<N; i++)
            {
                motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::DoubleEvent, i));
            
                MotionAnalysisEvent *ev = (MotionAnalysisEvent * ) sigz[0]->getMotionData().at(i);
                setMotionAnalysisMinMax(i, ev->getMin(), ev->getMax());
                motionData[i]->setName(ev->getName());
            }
        };
        
        virtual void updateMotionData(int k, double avgMotionData)
        {
            ( (MotionAnalysisEvent *)motionData[k])->setValue(avgMotionData);
        };
        
        virtual void update(float seconds = 0)
        {
            assert(signals.size() > 0 );
            
            int N = signals[0]->getMotionData().size();
            for(int k=0; k<N; k++)
            {
                double avgMotionData = 0;
                for( int i=0; i<signals.size(); i++ )
                {
                    avgMotionData += ((MotionAnalysisEvent *) signals[i]->getMotionData().at(k))->asDouble();
                }
                avgMotionData /= signals.size();
            
                updateMotionData(k, avgMotionData);
            }
        };
        
        //no OSC output
        std::vector< ci::osc::Message > getOSC(){  std::vector< ci::osc::Message > msgs; return msgs; } // erp for now
    };
    
//    class MotionDataAverageDiscountZeros
    
    
    //for int values -- like perceptual schemas
    class MotionIntegerDataAverage : public MotionDataAverage
    {
    public:
        MotionIntegerDataAverage( std::vector<SignalAnalysisEventOutput *> sigz ) : MotionDataAverage()
        {
            signals = sigz;
            
            int N = sigz[0]->getMotionData().size();
            
            for(int i=0; i<N; i++)
            {
                motionData.push_back(new MotionAnalysisEvent(MotionAnalysisDataType::IntEvent, i));
                
                MotionAnalysisEvent *ev = (MotionAnalysisEvent * ) sigz[0]->getMotionData().at(i);
                setMotionAnalysisMinMax(i, ev->getMin(), ev->getMax());
                motionData[i]->setName(ev->getName());

            }
        };
        
        
        virtual void updateMotionData(int k, double avgMotionData)
        {
            ( (MotionAnalysisEvent *)motionData[k])->setValue(std::round(avgMotionData));
        };

    };

    


};

#endif
