#include "sobreposicao.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"

#define EPSILON 1e-10

static double distancia_quadrada(double x1, double y1, double x2, double y2) {
    double deltaX = x1 - x2;
    double deltaY = y1 - y2;
    return (deltaX * deltaX) + (deltaY * deltaY);
}

static int orientacao(double px, double py, double qx, double qy, double rx, double ry) {
    double val = (qx - px) * (ry - py) - (qy - py) * (rx - px);
    if (fabs(val) < EPSILON) return 0;
    return (val > 0) ? 1 : 2;
}

static bool pontoEstaNoSegmento(double px, double py, double qx, double qy, double rx, double ry) {
    return (qx <= fmax(px, rx) && qx >= fmin(px, rx) && qy <= fmax(py, ry) && qy >= fmin(py, ry));
}

static bool sobrepoe_circulo_circulo(void *c1, void *c2) {
    double x1 = get_x((Circulo*)c1);
    double y1 = get_y((Circulo*)c1);
    double r1 = get_raio((Circulo*)c1);
    double x2 = get_x((Circulo*)c2);
    double y2 = get_y((Circulo*)c2);
    double r2 = get_raio((Circulo*)c2);
    double distSq = distancia_quadrada(x1, y1, x2, y2);
    double radSumSq = (r1 + r2) * (r1 + r2);
    return distSq <= radSumSq;
}

static bool sobrepoe_retangulo_retangulo(void *r1, void *r2) {
    double x1 = get_x_retangulo((Retangulo*)r1);
    double y1 = get_y_retangulo((Retangulo*)r1);
    double w1 = get_largura((Retangulo*)r1);
    double h1 = get_altura((Retangulo*)r1);
    
    double x2 = get_x_retangulo((Retangulo*)r2);
    double y2 = get_y_retangulo((Retangulo*)r2);
    double w2 = get_largura((Retangulo*)r2);
    double h2 = get_altura((Retangulo*)r2);

    bool colX = (x1 < x2 + w2) && (x1 + w1 > x2);
    bool colY = (y1 < y2 + h2) && (y1 + h1 > y2);
    return colX && colY;
}

static bool sobrepoe_circulo_retangulo(void *c, void *r) {
    double cx = get_x((Circulo*)c);
    double cy = get_y((Circulo*)c);
    double cr = get_raio((Circulo*)c);
    
    double rx = get_x_retangulo((Retangulo*)r);
    double ry = get_y_retangulo((Retangulo*)r);
    double rw = get_largura((Retangulo*)r);
    double rh = get_altura((Retangulo*)r);

    double testX = cx;
    double testY = cy;

    if (cx < rx) testX = rx;
    else if (cx > rx + rw) testX = rx + rw;
    
    if (cy < ry) testY = ry;
    else if (cy > ry + rh) testY = ry + rh;

    double distSq = distancia_quadrada(cx, cy, testX, testY);
    return distSq <= (cr * cr);
}

static bool sobrepoe_linha_linha(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    int o1 = orientacao(x1, y1, x2, y2, x3, y3);
    int o2 = orientacao(x1, y1, x2, y2, x4, y4);
    int o3 = orientacao(x3, y3, x4, y4, x1, y1);
    int o4 = orientacao(x3, y3, x4, y4, x2, y2);

    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && pontoEstaNoSegmento(x1, y1, x3, y3, x2, y2)) return true;
    if (o2 == 0 && pontoEstaNoSegmento(x1, y1, x4, y4, x2, y2)) return true;
    if (o3 == 0 && pontoEstaNoSegmento(x3, y3, x1, y1, x4, y4)) return true;
    if (o4 == 0 && pontoEstaNoSegmento(x3, y3, x2, y2, x4, y4)) return true;
    return false;
}

static bool sobrepoe_circulo_linha_coords(void *c, double x1, double y1, double x2, double y2) {
    double cx = get_x((Circulo*)c);
    double cy = get_y((Circulo*)c);
    double cr = get_raio((Circulo*)c);

    double d1 = distancia_quadrada(cx, cy, x1, y1);
    double d2 = distancia_quadrada(cx, cy, x2, y2);
    double r2 = cr * cr;

    if (d1 <= r2 || d2 <= r2) return true;

    double lenSq = distancia_quadrada(x1, y1, x2, y2);
    if (lenSq == 0.0) return d1 <= r2;

    double t = ((cx - x1) * (x2 - x1) + (cy - y1) * (y2 - y1)) / lenSq;
    t = fmax(0.0, fmin(1.0, t));
    
    double projX = x1 + t * (x2 - x1);
    double projY = y1 + t * (y2 - y1);
    
    return distancia_quadrada(cx, cy, projX, projY) <= r2;
}

static bool sobrepoe_retangulo_linha_coords(void *r, double x1, double y1, double x2, double y2) {
    double rx = get_x_retangulo((Retangulo*)r);
    double ry = get_y_retangulo((Retangulo*)r);
    double rw = get_largura((Retangulo*)r);
    double rh = get_altura((Retangulo*)r);

    if (x1 >= rx && x1 <= rx + rw && y1 >= ry && y1 <= ry + rh) return true;
    if (x2 >= rx && x2 <= rx + rw && y2 >= ry && y2 <= ry + rh) return true;

    bool s1 = sobrepoe_linha_linha(x1, y1, x2, y2, rx, ry, rx + rw, ry);
    bool s2 = sobrepoe_linha_linha(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh); 
    bool s3 = sobrepoe_linha_linha(x1, y1, x2, y2, rx + rw, ry + rh, rx, ry + rh);
    bool s4 = sobrepoe_linha_linha(x1, y1, x2, y2, rx, ry + rh, rx, ry);

    return s1 || s2 || s3 || s4; 
}

static void get_coords_texto(void *t, double *x1, double *y1, double *x2, double *y2) {
    double xt = get_x_texto((Texto*)t);
    double yt = get_y_texto((Texto*)t);
    char anc = get_anchor_texto((Texto*)t);
    const char* cont = get_conteudo_texto((Texto*)t);
    int len = cont ? strlen(cont) : 0;
    double comp = 10.0 * len;
    
    *y1 = yt; *y2 = yt;
    if (anc == 'i') { *x1 = xt; *x2 = xt + comp; }
    else if (anc == 'm') { *x1 = xt - comp/2.0; *x2 = xt + comp/2.0; }
    else { *x1 = xt - comp; *x2 = xt; } 
}

static bool sobrepoe_generico_linha(void *dados, TipoForma tipo, double x1, double y1, double x2, double y2) {
    switch(tipo) {
        case TIPO_CIRCULO: return sobrepoe_circulo_linha_coords(dados, x1, y1, x2, y2);
        case TIPO_RETANGULO: return sobrepoe_retangulo_linha_coords(dados, x1, y1, x2, y2);
        case TIPO_LINHA: {
            double lx1 = get_x1_linha((Linha*)dados);
            double ly1 = get_y1_linha((Linha*)dados);
            double lx2 = get_x2_linha((Linha*)dados);
            double ly2 = get_y2_linha((Linha*)dados);
            return sobrepoe_linha_linha(lx1, ly1, lx2, ly2, x1, y1, x2, y2);
        }
        case TIPO_TEXTO: {
            double tx1, ty1, tx2, ty2;
            get_coords_texto(dados, &tx1, &ty1, &tx2, &ty2);
            return sobrepoe_linha_linha(tx1, ty1, tx2, ty2, x1, y1, x2, y2);
        }
    }
    return false;
}

bool formas_sobrepoem(void *dados1, TipoForma tipo1, void *dados2, TipoForma tipo2) {
    if (tipo1 == TIPO_TEXTO) {
        double tx1, ty1, tx2, ty2;
        get_coords_texto(dados1, &tx1, &ty1, &tx2, &ty2);
        return sobrepoe_generico_linha(dados2, tipo2, tx1, ty1, tx2, ty2);
    }
    if (tipo2 == TIPO_TEXTO) {
        double tx1, ty1, tx2, ty2;
        get_coords_texto(dados2, &tx1, &ty1, &tx2, &ty2);
        return sobrepoe_generico_linha(dados1, tipo1, tx1, ty1, tx2, ty2);
    }
    if (tipo1 == TIPO_LINHA) {
        double lx1 = get_x1_linha((Linha*)dados1);
        double ly1 = get_y1_linha((Linha*)dados1);
        double lx2 = get_x2_linha((Linha*)dados1);
        double ly2 = get_y2_linha((Linha*)dados1);
        return sobrepoe_generico_linha(dados2, tipo2, lx1, ly1, lx2, ly2);
    }
    if (tipo2 == TIPO_LINHA) {
        double lx1 = get_x1_linha((Linha*)dados2);
        double ly1 = get_y1_linha((Linha*)dados2);
        double lx2 = get_x2_linha((Linha*)dados2);
        double ly2 = get_y2_linha((Linha*)dados2);
        return sobrepoe_generico_linha(dados1, tipo1, lx1, ly1, lx2, ly2);
    }

    if (tipo1 == TIPO_CIRCULO && tipo2 == TIPO_CIRCULO) return sobrepoe_circulo_circulo(dados1, dados2);
    if (tipo1 == TIPO_RETANGULO && tipo2 == TIPO_RETANGULO) return sobrepoe_retangulo_retangulo(dados1, dados2);
    if (tipo1 == TIPO_CIRCULO && tipo2 == TIPO_RETANGULO) return sobrepoe_circulo_retangulo(dados1, dados2);
    if (tipo1 == TIPO_RETANGULO && tipo2 == TIPO_CIRCULO) return sobrepoe_circulo_retangulo(dados2, dados1);
    
    return false;
}