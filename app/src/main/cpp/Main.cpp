#include <jni.h>
#include <android/log.h>
#include <thread>
#include <mutex>
#include <vector>
#include <cmath>
#include <atomic>
#include <KittyMemory/KittyMemory.h>
#include <Dobby/dobby.h>
#include "FeatureModule.h"
#include "Canvas.h"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"FFMenu",__VA_ARGS__)

#define OFF_WORLD_TO_SCREEN     0x0
#define OFF_GET_HEAD_COLLIDER   0x0
#define OFF_SET_AIM_ROTATION    0x0
#define OFF_GET_RECOIL          0x0
#define OFF_GET_VELOCITY        0x0

uintptr_t il2cpp_base = 0;
struct Vector3 { float x,y,z; };

void*   (*orig_world_to_screen)(void*,Vector3,void*) = nullptr;
void*   (*orig_get_head_collider)(void*,void*)       = nullptr;
void    (*orig_set_aim_rotation)(void*,Vector3,void*)= nullptr;
float   (*orig_get_recoil)(void*,void*)              = nullptr;
Vector3 (*orig_get_velocity)(void*,void*)            = nullptr;

struct ESPEntry{float x,y,w,h;};
std::mutex esp_mutex;
std::vector<ESPEntry> esp_back;
std::atomic<bool> running{true};

void* hook_world_to_screen(void* cam, Vector3 p, void* m){
    void* r = orig_world_to_screen(cam,p,m);
    Vector3* s = (Vector3*)&r;
    if(esp_enabled && s->z > 0.f){
        float sc = 500.f / s->z;
        std::lock_guard<std::mutex> lk(esp_mutex);
        esp_back.push_back({s->x, s->y, 80.f*sc, 160.f*sc});
    }
    return r;
}
void* hook_get_head_collider(void* pl, void* m){
    void* c = orig_get_head_collider(pl,m);
    return c;
}
void hook_aim_rotation(void* self, Vector3 rot, void* m){
    orig_set_aim_rotation(self,rot,m);
}
float hook_get_recoil(void* w, void* m){
    if(norecoil_enabled) return 0.f;
    return orig_get_recoil(w,m);
}

static void aim_at(Vector3 tgt, Vector3 tv, Vector3 cam, float ps, void* lp){
    float dx=tgt.x-cam.x, dy=tgt.y-cam.y, dz=tgt.z-cam.z;
    float d=std::sqrt(dx*dx+dy*dy+dz*dz);
    float t=(ps>1.f)?d/ps:0.f;
    float ax=tgt.x+tv.x*t-cam.x, ay=tgt.y+tv.y*t-cam.y, az=tgt.z+tv.z*t-cam.z;
    float len=std::sqrt(ax*ax+ay*ay+az*az);
    if(len<0.001f) return;
    const float R2D=180.f/3.14159265f;
    Vector3 rot{-std::asin(ay/len)*R2D, std::atan2(ax,az)*R2D, 0.f};
    orig_set_aim_rotation(lp,rot,nullptr);
}

static void render_loop(){
    Canvas::Init();
    while(running){
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        std::vector<ESPEntry> frame;
        { std::lock_guard<std::mutex> lk(esp_mutex);
          frame.swap(esp_back); esp_back.clear(); }
        for(auto& e : frame){
            Canvas::DrawBox(e.x-e.w/2, e.y-e.h/2, e.w, e.h, 2.f, 0xFF00FF00);
            Canvas::DrawLine(e.x, e.y-e.h/2, e.x, e.y+e.h/2, 1.f, 0xFFFF0000);
        }
    }
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* r){
    il2cpp_base = KittyMemory::getBaseAddress("libil2cpp.so");
    if(!il2cpp_base){ LOGI("libil2cpp.so missing"); return JNI_ERR; }
    LOGI("base 0x%lx", il2cpp_base);
    struct{const char* n; uintptr_t o;} chk[]={
        {"w2s",OFF_WORLD_TO_SCREEN},{"head",OFF_GET_HEAD_COLLIDER},
        {"aim",OFF_SET_AIM_ROTATION},{"rec",OFF_GET_RECOIL}};
    for(auto& c : chk){
        if(!c.o){ LOGI("%s offset zero", c.n); return JNI_ERR; }
        uint8_t b=*(uint8_t*)(il2cpp_base+c.o);
        if(b==0x00||b==0xFF){ LOGI("%s invalid 0x%02x", c.n, b); return JNI_ERR; }
    }
    DobbyHook((void*)(il2cpp_base+OFF_WORLD_TO_SCREEN),
              (void*)hook_world_to_screen,(void**)&orig_world_to_screen);
    DobbyHook((void*)(il2cpp_base+OFF_GET_HEAD_COLLIDER),
              (void*)hook_get_head_collider,(void**)&orig_get_head_collider);
    DobbyHook((void*)(il2cpp_base+OFF_SET_AIM_ROTATION),
              (void*)hook_aim_rotation,(void**)&orig_set_aim_rotation);
    DobbyHook((void*)(il2cpp_base+OFF_GET_RECOIL),
              (void*)hook_get_recoil,(void**)&orig_get_recoil);
    feature_init();
    std::thread(render_loop).detach();
    LOGI("hooks ok");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_anon_ffmenu_MainActivity_toggle(JNIEnv*, jobject, jint id, jboolean on){
    switch(id){ case 1: esp_enabled=on; break;
                case 2: aimbot_enabled=on; break;
                case 3: norecoil_enabled=on; break; }
}
