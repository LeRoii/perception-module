/*
 * Copyright (c) 2016-2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <queue>
#include "NvJpegDecoder.h"
#include "NvEglRenderer.h"
#include "NvUtils.h"
#include "NvCudaProc.h"
#include "nvbuf_utils.h"
#include <condition_variable>

#define V4L2_BUFFERS_NUM    4

#define INFO(fmt, ...) \
    if (ctx->enable_verbose) \
        printf("INFO: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define WARN(fmt, ...) \
        printf("WARN: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define CHECK_ERROR(cond, label, fmt, ...) \
    if (!cond) { \
        error = 1; \
        printf("ERROR: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        goto label; \
    }

#define ERROR_RETURN(fmt, ...) \
    do { \
        printf("ERROR: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return false; \
    } while(0)

typedef struct
{
    /* User accessible pointer */
    unsigned char * start;
    /* Buffer length */
    unsigned int size;
    /* File descriptor of NvBuffer */
    int dmabuff_fd;
} nv_buffer;

typedef struct
{
    /* Camera v4l2 context */
    const char * cam_devname;
    char cam_file[16];
    int cam_fd;
    unsigned int cam_pixfmt;
    unsigned int cam_w;
    unsigned int cam_h;
    unsigned int frame;
    unsigned int save_n_frame;

    /* Global buffer ptr */
    nv_buffer * g_buff;
    bool capture_dmabuf;

    /* EGL renderer */
    NvEglRenderer *renderer;
    int render_dmabuf_fd;
    int fps;

    /* CUDA processing */
    bool enable_cuda;
    EGLDisplay egl_display;
    EGLImageKHR egl_image;

    /* MJPEG decoding */
    NvJPEGDecoder *jpegdec;

    /* Verbose option */
    bool enable_verbose;

} context_t;

/* Correlate v4l2 pixel format and NvBuffer color format */
typedef struct
{
    unsigned int v4l2_pixfmt;
    NvBufferColorFormat nvbuff_color;
} nv_color_fmt;

static std::mutex m_mtx[2];
static std::condition_variable con[2];

class v4l2{
public: 
    v4l2();
    v4l2(const int camtype);
    ~v4l2();
    bool start_capture(std::vector<cv::Mat> &buffer_thermel);
    bool start_capture();
    bool stream_init();
    bool display_init();
    bool display(cv::Mat &mat);
    void run();
    int getFrame(cv::Mat &mat, bool src = true);
    cv::Mat m_ret;

private:
    void set_defaults(context_t * ctx);
    NvBufferColorFormat get_nvbuff_color_fmt(unsigned int v4l2_pixfmt);
    bool camera_initialize(context_t * ctx);
    bool display_initialize(context_t * ctx);
    bool camera_init_components(context_t * ctx);
    bool display_init_components(context_t * ctx);
    bool request_camera_buff(context_t * ctx);
    bool request_camera_buff_mmap(context_t * ctx);
    bool prepare_buffers(context_t * ctx);
    bool start_stream(context_t * ctx);
    bool stop_stream(context_t * ctx);
    

private:
    nv_buffer *retNvbuf;
    cv::Mat m_argb;
    std::vector<cv::Mat> frame_buffer;
    context_t ctx;
    int m_type;
    int m_id;
    
    std::queue<cv::Mat> m_queue;
    bool m_stop;
};
