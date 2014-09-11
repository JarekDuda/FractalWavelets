/* 
 * File:   frave.cpp
 * Author: Jarek
 *
 * Created on September 10, 2014, 1:35 PM
 */

#include <cstdlib>
#include <vector>
#include <iostream>
#include "bitmap_image.hpp"

using namespace std;
typedef uint32_t uint;
typedef unsigned char byte;

struct coord {int x,y;
coord operator+(const coord & b) const {return coord({x + b.x, y + b.y});}
};
            // choose variant
const coord v[]=                      // D = -1 , added first vector - TAME TWINDRAGON
{{0, 1}, {2, 0}, {-1, 1}, {-3, -1}, {5, -1}, {1, 3}, {-11, -1}, {9, -5}, {13, 7}, {-31, 3}, 
{5, -17}, {57, 11}, {-67, 23}, {-47, -45}, {181, -1}, {-87, 91}, {-275, -89}, {449, -93}, 
{101, 271}, {-999, -85}, {797, -457}, {1201, 627}, {-2795, 287}, {393, -1541}, {5197, 967},
{-5983, 2115}, {-4411, -4049}, {16377, -181}, {-7555, 8279}, {-25199, -7917}, {40309, -8641}};
//const coord v[]=                  // D = -1 for  {1,1}, {1,-1} base - skewed TAME TWINDRAGON
//{{1, 1}, {-1, 0}, {-1, -2}, {3, 2}, {-1, 2}, {-5, -6}, {7, 2}, {3, 10}, {-17, -14}, {11, -6}, 
//{23, 34}, {-45, -22}, {-1, -46}, {91, 90}, {-89, 2}, {-93, -182}, {271, 178}, {-85, 186}, 
//{-457, -542}, {627, 170}, {287, 914}, {-1541, -1254}, {967, -574}, {2115, 3082}, {-4049, -1934}, 
//{-181, -4230}, {8279, 8098}, {-7917, 362}, {-8641, -16558}, {24475, 15834}};
//const coord v[]=                  // D = -2       - TWINDRAGON
//{{1, 0}, {-1, 1}, {0, -2}, {2, 2}, {-4, 0}, {4, -4}, {0, 8}, {-8, -8}, {16, 0}, 
//{-16, 16}, {0, -32}, {32, 32}, {-64, 0}, {64, -64}, {0, 128}, {-128, -128}, {256, 0}, 
//{-256, 256}, {0, -512}, {512, 512}, {-1024, 0}, {1024, -1024}, {0, 2048}, {-2048, -2048}, 
//{4096, 0}, {-4096, 4096}, {0, -8192}, {8192, 8192}, {-16384, 0}, {16384, -16384}, {0, 32768}};
//const coord v[]=                  // D = 0          - BOXES
//  {{1, 0}, {0, 1}, {-2, 0}, {0, -2}, {4, 0}, {0, 4}, {-8, 0}, {0, -8}, {16, 0}, {0, 16}, 
//  {-32, 0}, {0, -32}, {64, 0}, {0, 64}, {-128, 0}, {0, -128}, {256, 0}, {0, 256}, {-512, 0},
//  {0, -512}, {1024, 0}, {0, 1024}, {-2048, 0}, {0, -2048}, {4096, 0}, {0, 4096}, {-8192, 0}, 
//  {0, -8192}, {16384, 0}, {0, 16384}, {-32768, 0}};

class frave{
public:
    int w, h, maxx, minx, maxy, miny, depth;
    coord center;
    bitmap_image * img;
    uint *coef;                                  // Haar coefficients in hash-like tree, [0] is sum of all
    
    frave(bitmap_image &im){              // finds center and depth
        w=im.width(); h=im.height(); img = &im; center.x=0; center.y=0;
        int rw=w, rh=h, dp=0;
        do {rw-=abs(v[dp].x); rh-=abs(v[dp++].y);} while ((rw >= 0) && (rh>=0));    // find depth
        depth = dp-1;                                                  // we use v[0] to v[depth-1]  
        coef = new uint[1 << depth];
        for(dp=0; dp<depth; dp++)                                                  // find center
            {coord c=v[dp];  center.x -= (c.x>0 ? 0: c.x); center.y -= (c.y>0 ? 0 : c.y);}            
    cout<<" w: "<<w<<" h: "<<h<<" center.x: "<<center.x<<" center.y: "<<center.y<<" depth: "<<depth;
    }  
    ~frave(){delete [] coef;}
    inline uint getpixel(int x, int y){byte r,g,b; img->get_pixel(x,y,r,g,b); return r;}       // assumes grayscale
    inline void setpixel(int x, int y, int v){byte g = max(0,min(v,255)); img->set_pixel(x,y,g,g,g);}
    void trim_coef(int dp){for(int i= (1<<dp); i<(1<<depth); i++) *(coef+i)=0;}
    
    // ---------------- TRANSFORMS ------------------
    void find_coef(){                               // values -> coefficients
        int lt=fn_cf(center, 2, depth-2), rt=fn_cf(center + v[depth-1], 3, depth-2);
        *(coef+1)= rt-lt; *coef = rt + lt;
    }
   int fn_cf(coord cn, int ps, int dp){             // recurrence
        int lt, rt;
        if(dp){lt=fn_cf(cn, ps<<1, dp-1); rt=fn_cf(cn + v[dp], (ps<<1)+1, dp-1);} 
        else {lt=getpixel(cn.x, cn.y); rt=getpixel(cn.x + v[0].x, cn.y + v[0].y);}
        *(coef+ps) = rt-lt; return rt+lt;
    }
    void find_val(){fn_vl(*coef, 1, center, depth-1); }       // coefficients -> values
    void fn_vl(int sum, int ps, coord cn, int dp){                 // recurrence 
        int dif = *(coef+ps), lt = (sum - dif)>>1, rt = (sum + dif)>>1;
        if(dp){fn_vl(lt, ps<<1, cn, dp-1); fn_vl(rt, (ps<<1)+1, cn+v[dp], dp-1);}
        else {setpixel(cn.x,cn.y,lt); setpixel(cn.x + v[0].x, cn.y + v[0].y,rt);} 
    }
};

int main(int argc, char** argv) {
    bitmap_image image("input.bmp");
    frave fr(image);
    fr.find_coef();                         // values -> coefficients
    fr.trim_coef(12);                       // trim to first 2^k coefficients (higher set to zero)
    fr.find_val();                          // coefficients -> values
    image.save_image("output.bmp");
    return 0;
}
