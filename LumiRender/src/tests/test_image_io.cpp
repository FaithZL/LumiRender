//
// Created by Zero on 2021/2/20.
//

#include "util/image_io.h"
#include "iostream"
using namespace luminous;
using namespace std;



int main() {

    auto path = "E:\\work\\graphic\\renderer\\LumiRender\\LumiRender\\res\\image\\HelloWorld2.png";
    auto path2 = "E:\\work\\graphic\\renderer\\LumiRender\\LumiRender\\res\\image\\HelloWorld3.bmp";

    auto [rgb, res] = load_image(path);

    save_image(path2, rgb, res);

    return 0;
}