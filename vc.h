//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITÉCNICO DO CÁVADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORMÁTICOS
//                    VISÃO POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define VC_DEBUG

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct {
    unsigned char *data;
    int width, height;
    int channels;            // Binário/Cinzentos=1; RGB=3
    int levels;              // Binário=1; Cinzentos [1,255]; RGB [1,255]
    int bytesperline;        // width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UM BLOB (OBJETO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef struct {
    int x, y, width, height;    // Caixa Delimitadora (Bounding Box)
    int area;                   // Área
    int xc, yc;                 // Centro-de-massa
    int perimeter;              // Perímetro
    int label;                  // Etiqueta
} OVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROTÓTIPOS DE FUNÇÕES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUNÇÕES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);

IVC *vc_image_free(IVC *image);

// FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);

int vc_write_image(char *filename, IVC *image);

//
// Created by Helder Carvalho on 07/03/2021.
//

// FUNCTIONS GRAY
int vc_gray_negative(IVC *src_dst);

int vc_scale_gray_to_rgb(IVC *src, IVC *dst);

int vc_gray_to_binary(IVC *src, IVC *dst, int threshold);

int vc_gray_to_binary_global_mean(IVC *src, IVC *dst);

int vc_gray_to_binary_neighborhood_midpoint(IVC *src, IVC *dst, int kernel);

int vc_gray_histogram_show(IVC *src, IVC *dst);

int vc_gray_histogram_equalization(IVC *src, IVC *dst);

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernel_size);

int vc_gray_lowpass_median_filter(IVC *src, IVC *dst, int kernel_size);

int vc_gray_lowpass_gaussian_filter(IVC *src, IVC *dst);

int vc_gray_highpass_filter(IVC *src, IVC *dst, int mask_type);

int vc_gray_highpass_filter_enhance(IVC *src, IVC *dst, int mask_type, int gain);

// FUNCTIONS RGB
int vc_rgb_negative(IVC *src_dst);

int vc_rgb_get_red_gray(IVC *src_dst);

int vc_rgb_get_green_gray(IVC *src_dst);

int vc_rgb_get_blue_gray(IVC *src_dst);

int vc_rgb_to_gray(IVC *src, IVC *dst);

int vc_rgb_to_hsv(IVC *src, IVC *dst);

// FUNCTIONS HSV
int vc_hsv_segmentation(IVC *src, IVC *dst, int h_min, int h_max, int s_min, int s_max, int v_min, int v_max);

// FUNCTIONS BINARY
int vc_binary_dilate(IVC *src, IVC *dst, int kernel);

int vc_binary_erode(IVC *src, IVC *dst, int kernel);

int vc_binary_open(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);

int vc_binary_close(IVC *src, IVC *dst, int kernel_erode, int kernel_dilate);

OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *n_labels);

int vc_binary_blob_info(IVC *src, OVC *blobs, int n_blobs);

int vc_gray_edge_prewitt(IVC *src, IVC *dst, float threshold);

int vc_gray_edge_sobel(IVC *src, IVC *dst, float threshold);
