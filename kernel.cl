//#pragma OPENCL EXTENSION cl_amd_printf : enable

#define coordToIndex(x, y, w) (x) + ((y) * (w))

void kernel kmain(global int *buffer0, global int *buffer1, int width, int height) {
    int threadID = get_global_id(0);
    int nThreads = get_global_size(0);

    global int *previousBuffer = buffer0;
    global int *newBuffer = buffer1;

    for (int y = threadID; y < height; y += nThreads) {
        for (int x = 0; x < width; x++) {
            int cellneighbors = 0;
            int cellstate = previousBuffer[coordToIndex(x, y, width)];

            //Faster
            if (y-1 >= 0 && x-1 >= 0) {        if (previousBuffer[coordToIndex(x-1, y-1, width)] & 1) ++cellneighbors; }
            if (y-1 >= 0) {                    if (previousBuffer[coordToIndex(x  , y-1, width)] & 1) ++cellneighbors; }
            if (y-1 >= 0 && x+1 < width) {     if (previousBuffer[coordToIndex(x+1, y-1, width)] & 1) ++cellneighbors; }
            if (x-1 >= 0) {                    if (previousBuffer[coordToIndex(x-1, y  , width)] & 1) ++cellneighbors; }
            //skip self                                                       (x  , y  )
            if (x+1 < width) {                 if (previousBuffer[coordToIndex(x+1, y  , width)] & 1) ++cellneighbors; }
            if (y+1 < height && x-1 >= 0) {    if (previousBuffer[coordToIndex(x-1, y+1, width)] & 1) ++cellneighbors; }
            if (y+1 < height) {                if (previousBuffer[coordToIndex(x  , y+1, width)] & 1) ++cellneighbors; }
            if (y+1 < height && x+1 < width) { if (previousBuffer[coordToIndex(x+1, y+1, width)] & 1) ++cellneighbors; }
            
            //More readable
            /*for (int oy = -1; oy < 2; oy++) {
                for (int ox = -1; ox < 2; ox++) {
                    if ((ox == 0) && (oy == 0)) {
                        continue;
                    }

                    if (((x + ox) < 0) ||
                        ((y + oy) < 0) ||
                        ((x + ox) > width) ||
                        ((y + oy) > width)) {
                            continue;
                    }

                    if (previousBuffer[coordToIndex(x+ox, y+oy, width)] != 0) {
                        ++cellneighbors;
                    }
                }
            }*/

            int newVal = cellstate;
            if (cellstate == 1) {
                if ((cellneighbors < 2) || (cellneighbors > 3)) newVal = 0;
            } else {
                if (cellneighbors == 3) newVal = 1;
            }
            newBuffer[coordToIndex(x, y, width)] = newVal;
        }
    }
}