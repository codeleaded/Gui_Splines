#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"

typedef struct sPoint2D {
    float x;
    float y;
} sPoint2D;

typedef struct sSpline {
    Vector points;
} sSpline;

sSpline path;
int nSelected = -1;
float Ship = 0.0f;

sPoint2D sSpline_GetSplinePoint(sSpline* s,float t,char bLooped){
    int p0,p1,p2,p3;
    if(!bLooped){
        p1 = (int)t+1;
        p2 = p1+1;
        p3 = p2+1;
        p0 = p1-1;
    }else{
        p1 = (int)t;
        p2 = (p1+1) % s->points.size;
        p3 = (p2+1) % s->points.size;
        p0 = p1>=1?p1-1:s->points.size-1;
    }
    t = t - (int)t;

    float tt = t*t;
    float ttt = tt*t;

    float q1 =         -ttt + 2.0f * tt - t;
    float q2 =  3.0f *  ttt - 5.0f * tt + 2.0f;
    float q3 = -3.0f *  ttt + 4.0f * tt + t;
    float q4 =          ttt -        tt;

    sPoint2D point1 = *(sPoint2D*)Vector_Get(&s->points,p0);
    sPoint2D point2 = *(sPoint2D*)Vector_Get(&s->points,p1);
    sPoint2D point3 = *(sPoint2D*)Vector_Get(&s->points,p2);
    sPoint2D point4 = *(sPoint2D*)Vector_Get(&s->points,p3);

    float tx = 0.5f * (point1.x * q1 + point2.x * q2 + point3.x * q3 + point4.x * q4);
    float ty = 0.5f * (point1.y * q1 + point2.y * q2 + point3.y * q3 + point4.y * q4);

    return (sPoint2D){tx,ty};
}
sPoint2D sSpline_GetSplineGradient(sSpline* s,float t,char bLooped){
    int p0,p1,p2,p3;
    if(!bLooped){
        p1 = (int)t+1;
        p2 = p1+1;
        p3 = p2+1;
        p0 = p1-1;
    }else{
        p1 = (int)t;
        p2 = (p1+1) % s->points.size;
        p3 = (p2+1) % s->points.size;
        p0 = p1>=1?p1-1:s->points.size-1;
    }
    t = t - (int)t;

    float tt = t*t;

    float q1 = -3.0f *  tt + 4.0f * t - 1.0f;
    float q2 =  9.0f *  tt -10.0f * t;
    float q3 = -9.0f *  tt + 8.0f * t + 1.0f;
    float q4 =  3.0f *  tt - 2.0f * t;

    sPoint2D point1 = *(sPoint2D*)Vector_Get(&s->points,p0);
    sPoint2D point2 = *(sPoint2D*)Vector_Get(&s->points,p1);
    sPoint2D point3 = *(sPoint2D*)Vector_Get(&s->points,p2);
    sPoint2D point4 = *(sPoint2D*)Vector_Get(&s->points,p3);

    float tx = 0.5f * (point1.x * q1 + point2.x * q2 + point3.x * q3 + point4.x * q4);
    float ty = 0.5f * (point1.y * q1 + point2.y * q2 + point3.y * q3 + point4.y * q4);

    return (sPoint2D){tx,ty};
}

void Setup(AlxWindow* w){
    path = (sSpline){Vector_New(sizeof(sPoint2D))};
    for(int i = 0;i<20;i++){
        Vector_Push(&path.points,(sPoint2D[]){(sPoint2D){i*10,i*5}});
    }
}
void Update(AlxWindow* w){
    if(Stroke(ALX_MOUSE_L).PRESSED){
        nSelected = -1;
        for(int i = 0;i<path.points.size;i++){
            sPoint2D* point = (sPoint2D*)Vector_Get(&path.points,i);
            float dx = point->x-GetMouse().x;
            float dy = point->y-GetMouse().y;
            if(sqrtf(dx*dx + dy*dy)<5.0f){
                nSelected = i;
            }
        }
    }
    if(Stroke(ALX_MOUSE_L).DOWN){
        if(nSelected>=0 && nSelected<path.points.size){
            sPoint2D* point = (sPoint2D*)Vector_Get(&path.points,nSelected);
            point->x = GetMouse().x;
            point->y = GetMouse().y;
        }
    }
    
    Clear(BLACK);

    //for(float t = 0.0f;t<(float)path.points.size-3.0f;t+=0.005f){
    //    sPoint2D pos = sSpline_GetSplinePoint(&path,t,FALSE);
    //    Point_Render(WINDOW_STD_ARGS,(Vec2){ pos.x,pos.y },WHITE);
    //}
    for(float t = 0.0f;t<(float)path.points.size;t+=0.005f){
        sPoint2D pos = sSpline_GetSplinePoint(&path,t,1);
        Point_Render(WINDOW_STD_ARGS,(Vec2){ pos.x,pos.y },WHITE);
    }

    for(int i = 0;i<path.points.size;i++){
        sPoint2D* point = (sPoint2D*)Vector_Get(&path.points,i);
        RenderRect(point->x-1,point->y-1,3,3,RED);
    }

    if(nSelected>=0 && nSelected<path.points.size){
        sPoint2D* selected = (sPoint2D*)Vector_Get(&path.points,nSelected);
        RenderRect(selected->x-1,selected->y-1,3,3,YELLOW);
    }

    sPoint2D spos = sSpline_GetSplinePoint(&path,Ship,1);
    sPoint2D sdir = sSpline_GetSplineGradient(&path,Ship,1);
    Vec2 pos = {spos.x,spos.y};
    if(sdir.x!=0.0f){
        float a = atan2f(-sdir.y,sdir.x);
        Vec2 p1 = { sinf(a), cosf(a)};
        Vec2 p2 = {-sinf(a),-cosf(a)};
        p1 = Vec2_Mulf(p1,10.0f);
        p2 = Vec2_Mulf(p2,10.0f);
        p1 = Vec2_Add(p1,pos);
        p2 = Vec2_Add(p2,pos);
        RenderLine(p1,p2,BLUE,1.0f);
    }

    Ship += 1.0f * w->ElapsedTime;
    Ship = Ship>path.points.size?0.0f:Ship;
}
void Delete(AlxWindow* w){
    Vector_Free(&path.points);
}

int main(){
    if(Create("Splines",1900,1000,1,1,Setup,Update,Delete))
        Start();
    return 0;
}