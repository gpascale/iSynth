
#include <GMath.h>
#include <Engine.h>

#include "PSMaster.h"
#ifdef WINDOWS
	#include "WindowsTopLayer.h"
#endif
#ifdef IPHONE
	#include "MacTopLayer.h"
#endif

#include <JsonSynthManager.h>

#ifdef WINDOWS

using namespace std;

/*
void test(D3DXVECTOR3 eye,
          D3DXVECTOR3 targ,
          D3DXVECTOR3 up)
{
    D3DXMATRIX m;
    D3DXMatrixLookAtLH(&m, &eye, &targ, &up);

    Matrix mm(m(0, 0), m(0, 1), m(0, 2), m(0, 3),
              m(1, 0), m(1, 1), m(1, 2), m(1, 3),
              m(2, 0), m(2, 1), m(2, 2), m(2, 3),
              m(3, 0), m(3, 1), m(3, 2), m(3, 3));

    Vector3 v3eye(eye.x, eye.y, eye.z);
    Vector3 v3targ(targ.x, targ.y, targ.z);

    //cout << "The matrix to look at " << v3targ << " from " << v3eye << " is " <<
        endl << mm << endl;
}
*/


/*
int main()
{   
    WindowsTopLayer *topLayer = new WindowsTopLayer();			
    topLayer->run();
    delete topLayer;
    while(1);
}
*/


#endif