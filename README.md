## Resumen

En este proyecto se remueve el fondo de un video en tiempo real. Se usaron técnicas vistas en clase como grafos y programación dinámica. El video del funcionamiento se puede encontrar [aquí](. https:/iteso01-my.sharepoint.com/:v:/g/personal/alejandro_lozano_iteso_mx/EacgptpSQQNLr2dw534zB9YBLijw7Nh7cZRfbVXRNFMy6g?e=89Ebsg).

El código se encuentra en GitHub https://github.com/alejandrolozano2/backgroundRemoval  y https://github.com/alejandrolozano2/BRemoval_Kruskal así como en el archivo. tar.gz junto a este documento. 



## Problema

Se necesita remover el fondo de un video en el caso cuando una persona está frente a la cámara. Ejemplos de esto se pueden observar en aplicaciones como Microsoft Teams. Este problema puede ser abordado usando técnicas de aprendizaje profundo pero este método está fuera del alcance de este documento e implementación. Debido a esto último se abordarán métodos vistos en clase como grafos, métodos de ordenamiento, programación dinámica etc. 



## Solución Propuesta

Dentro del proceso de búsqueda de solución se encontraron dos formas, una con mejores resultados que la otra. La primera forma fue tratando de segmentar las imágenes y la segunda se trató de determinar qué parte de las imágenes pertenecían a la persona por medio de contornos dentro de la grabación. 

Para ambos métodos se utilizó openCV para la captura de video en C++. Hay forma de modificar el formato de este obtenido por openCV, pero este en realidad depende de las características de la cámara. En este proyecto la resolución usada fue 640x480 pixeles. La herramienta openCV obtiene una serie de imágenes (video) en formato BGR (Azul, Verde, Rojo). La imagen obtenida es un arreglo de tres dimensiones (H x W x C) 640 x 480 (pixeles) x 3 canales (BGR). 

![image-20211204094956120](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204094956120.png)

Es decir que la entrada a nuestro algoritmo para remover el fondo consiste en 921600 pixeles y cada píxel tiene un tamaño de 4 bytes por lo que el tamaño en bytes de nuestra entrada es de 3,686,400 Bytes.  La salida es una serie de imágenes (video) del mismo formato. 



### Segmentación Kruskal

En esta primear implementación se trató de segmentar las imágenes obtenidas. La segmentación se realizó en base al color. Puede haber varios criterios de remover el fondo, pero en este caso el criterio usado fue el de identificar al fondo como el segmento más grande. Este método obtiene buenos resultados siempre y cuando el fondo sea uniforme, es decir que tenga el mismo color y sin objetos diferentes dentro de él. 

Para realizar esta segmentación se trabajó la imagen BGR como un grafo pesado, donde cada píxel con sus 3 valores BGR son considerados los nodos. Cada nodo tiene 4 aristas conectadas a los nodos superior, inferior, izquierdo y derecho (nodos vecinos). 

![image-20211204095243002](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204095243002.png)

El valor de cada arista es la diferencia absoluta del píxel central con sus nodos vecinos. 

```C++
#define bgrDiff(x, y)  (abs(diff((x)->red, (y)->red)) + abs(diff((x)->green, (y)->green)) + abs(diff((x)->blue, (y)->blue)))
```

La creación de las aristas se realiza en el método buildEdges. Cada objecto de la arista consta en realidad de 3 valores: los dos pixeles que conecta y su diferencia. Este método recibe de entrada la imagen en BGR y un arreglo sin inicializar de las aristas por construir. 

```c++
uint32_t buildEdges(cv::Mat & image, uint32_t (* e)[3], uint32_t radius) {
        cv::Size s = image.size();
        RGB * p = image.ptr<RGB>(0);
        uint32_t i = 0, j =0 , diff = 0, maxdiff= 0;

        for ( int r = 0; r < s.height; r++) {          
                for (int c = 0; c < s.width; c++) {
                        j = r * s.width + c;
                        if (c < s.width - 1 ) {
                                diff = bgrDiff(p, p + 1);            //Get difference with the pixel on the right
                                e[i][2] = diff;
                                e[i][1] = j + 1;                        //One pixel to the right
                                e[i++][0] = j;                          //Current Pixel
                                if ( diff >  maxdiff) maxdiff = diff;
                        }

                        if (r < s.height - 1 ) {
                                diff = bgrDiff(p, p + s.width);      //Get difference with the pixel below
                                e[i][2] = diff;
                                e[i][1] = j + s.width;                  //Pixel below
                                e[i++][0] = j;
                                if ( diff >  maxdiff) maxdiff = diff;
                        }
                        p++;
                }
        }

        return maxdiff;
}
```

Una vez contruidas las aristas, estas se ordenan de menor a mayor usando como criterio de ordenamiento el valor de la diferencia entre los pixeles. Para obetner un mejor rendimiento se usó el método por conteo. 

```c++
void sortEdges( uint32_t (*es)[3], uint32_t (*ed)[3], uint32_t size, uint32_t maxVal ) { 
    #define WINDEX 2

    uint32_t  count[maxVal + 1] = {};
    uint32_t  index = 0;

    for ( int i = 0; i < size; i++ ) 
        count[es[i][WINDEX]] += 1;
    
    for ( int i = 1; i <= maxVal; i++ ) 
        count[i] += count[i-1];
    
    for ( int i = size -1 ; i >= 0; i--) {
        count[es[i][WINDEX]] -= 1;
        index = count[es[i][WINDEX]];
        ed[index][0] = es[i][0];
        ed[index][1] = es[i][1];
        ed[index][2] = es[i][2];
    }
}
```

Una ves ordenados las aristas estas se usan como entrada para el algoritmo de kruskal y contruir el MST.  En este caso solo se unen los nodos hasta que se  llega a un umbral, es decir mientras las aristas y nodos que se van uniendo por medio del algoritmo de find and join tengan un valor no mayor a dicho umbral. 

```c++
uint32_t kruskal(uint32_t  (* edges)[3], uint32_t eSize, uint32_t pSize, uint32_t * parents, uint32_t * ranks, uint32_t threshold) {
    uint32_t j, k, w;
    uint32_t mergedCount = 0;

    for (int i = 0 ; i < eSize; i++) {
        j = edges[i][0];
        k = edges[i][1];
        w = edges[i][2];

        if ( w > threshold )
            break;

        if ( join(parents, ranks, pSize, j ,k) == true)
            mergedCount++;

    }

    return mergedCount;
}
```

Este algoritmo generará varios árboles no conexos del grafo original como se muestra a continuación

![image-20211204100736143](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204100736143.png)

Una vez creados los árboles se obtiene el árbol con mayor cantidad de nodos. Esto es se obtiene por medio de el arreglo de ranks. Con esta información es posible barrer pixel por pixel en la imagen y poner como 0 los pixeles que pertenezcan al árbol más grande. 

```c++
				auto max = buildEdges(colorMat, (uint32_t(*)[3])edges, 0);
                sortEdges((uint32_t(*)[3])edges, (uint32_t(*)[3])sortedEdges, nEdges, max);
                mergedCount = kruskal((uint32_t(*)[3])sortedEdges, nEdges, imageSize, parents, ranks, threshold);
                maxLeader = getMaxParent(ranks, imageSize);
                RGB * p = colorMat.ptr<RGB>(0);
                for (int i = 0 ; i < imageSize; i++, p++) {
                        uint32_t l = findLeader(parents, imageSize, i);
                        if (l == maxLeader) {
                                p->blue = 0;
                                p->green = 0;
                                p->red = 0;
                        }
                }
```

Este método como se mencionó funciona muy bien cuando el fondo es uniforme. En el caso de que se encuentre un objeto por ejemplo en una pared, este objeto no es removido.  Por lo que se implementó una segunda forma para remover el fondo.

### Contornos (DFS, PD)

Esta segunda implementación realiza los siguientes pasos generales:

* Obtener la imagen en BGR
* Generar imagen en escala de grises
* Obtener contornos de la imagen en escala de grises
* Acentuar contornos importantes y remover contornos débiles
* Detectar ojos de la persona en la imagen en escala de grises
* Una vez detectados los ojos, buscar los contornos en una periperia cercana (DFS)
* Colorerar/llenar contornos cerrados (PD). Esto genera segmentos blancos de la persona y objetos en el fondo. 
* Se asume que el contorno/segmento generado por el DFS y PD es parte de la persona y se obtiene una máscara de los demás objetos en el fondo. 
* Se pinta de negro todo aquel pixel que no pertenezca al contorno/segmento en la cercanía de los ojos. 
* En el caso de colisión del segmento de la persona y un objeto en el fondo, se usa la máscara obtenida para determinar si pintar de negro ese píxel o respetar su valor actual. 



Para la conversión a escala de grises simplemente se utilizó la siguiente fórmula sobre cada píxel BGR -> 0.299 ∙ Red + 0.587 ∙ Green + 0.114. 

```c++
int bgr2grey (cv::Mat & src, cv::Mat & dst) {
        cv::Size s = src.size();

        uint8_t * gImage = dst.ptr<uint8_t>(0);
        RGB * data = src.ptr<RGB>(0);
        for ( int h = 0; h < s.height; h++) {
                for (int w = 0; w < s.width; w++) {
                        *gImage++ = (data->blue* 0.114 + data->green* 0.587 + data->red* 0.299);
                        data++;
                }
        }
        return  0;
}
```

Ya que se tiene la imagen en escala de grises, se deriva en ambos sentidos para obtener los contornos. A esta operación se le llama Sobel.

```c++
int Sobel (cv::Mat & src, cv::Mat & dst, cv::Mat  & angles) {
        cv::Size s = src.size();

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        float (* aImage)[s.width] = (float (*)[s.width])angles.ptr<float>(0);
        float x, y, a;
        for ( int h = 1; h < s.height-1; h++) {
                for (int w = 1; w < s.width-1; w++) {
                        x =     -1.0 * (float)sImage[h-1][w-1] + (float)sImage[h-1][w+1] + 
                                -2.0*(float)sImage[h][w-1] + 2.0*(float)sImage[h][w+1]    +
                                -1.0*(float)sImage[h+1][w-1] + (float)sImage[h+1][w+1];
                        y =     -1.0 * (float)sImage[h-1][w-1] + -2.0*(float)sImage[h-1][w] + -1*(float)sImage[h-1][w+1] + 
                                (float)sImage[h+1][w-1] + 2.0*(float)sImage[h+1][w] + (float)sImage[h+1][w+1];
                        bImage[h][w] = sqrt(x*x + y*y);
                        a = atan2(y,x) * 180.0 / M_PI;
                        aImage[h][w] = a < 0 ? a + 180.0 : a; 
                }
        }

        return  0;
}
```

Se utiliza otro método para acentuar dichos contornos y eliminar contornos que pueden no ayudar a la detacción de la persona. 

```c++
int mthreshold (cv::Mat & src, cv::Mat & dst, float lowRatio, float highRatio, uint8_t max) {
        cv::Size s = src.size();
        uint8_t width = 2;
        uint8_t weak = 0;
        uint8_t strong = 255;

        uint8_t highTh = (uint8_t)highRatio;//max * highRatio;
        uint8_t lowTh = (uint8_t)lowRatio;//highTh * lowRatio;

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        
        for ( int h = 0; h < s.height; h++) {
                for (int w = 0; w < s.width; w++) {

                        if ( (h >= 0 && h <= width) || (h < s.height && h >= s.height - width) ||
                                (w >= 0 && w <= width) || (w < s.width && w >= s.width - width ) ) {
                                        bImage[h][w] = 0;
                                        continue;
                        }

                        if (sImage[h][w] >= highTh ) {
                                bImage[h][w] = strong;
                        } else if (sImage[h][w] < highTh && sImage[h][w] >= lowTh) {
                                bImage[h][w] = weak;
                        } else {
                                bImage[h][w] = 0;
                        }       
                }
        }
        return 0;
}
```

La salida en este momento sería una imagen con pixeles solo en negros o blancos (pertenecientes a contornos) como se muestra a continuacíón:

![image-20211204112540350](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204112540350.png)

Para la detección de los ojo se utilizó un método de openCV llamado matchTemplate. Una vez detectados los ojos se obtienen los contornos usando los pixeles más cercanos a los ojos. Esto usa una búsqueda en profundidad, usando como nodos solo a los pixeles vecinos que estén en 255, es decir que sean parte de un contorno. Este método recibe como entrada el grafo que es la imagen con todos los contornos acentuados, el inicio que es la posición del pixel donde se encontraron los ojos y fin corresponde a un valor de pixel no existente en la imagen para que recorra todos los posibles nodos/pixeles que sean parte del contorno más cercano. 

```c++
uint32_t DFS(cv::Mat & image, cv::Mat & out, uint32_t start, uint32_t end) {
        
        uint32_t pixelCount = 0;
        cv::Size s = image.size();
        uint8_t * sImage = image.ptr<uint8_t>(0);
        uint8_t * outImage = out.ptr<uint8_t>(0);
        uint32_t cPixel;
        int32_t row, col;
        list <uint32_t> stack;

        if ( *(sImage + start) == WHITE) 
                stack.push_front(start);

        while (!stack.empty()) {
                cPixel = stack.front();
                stack.pop_front();

                if (cPixel == end)
                        return pixelCount;

                row = cPixel / s.width;
                col = cPixel % s.width;

                if (*(outImage + cPixel) == 0) {
                        pixelCount += 1;
                        *(outImage + cPixel) = WHITE;

                        if ( row - 1 >= 0 ) {
                                if ( *(sImage + cPixel - s.width) == WHITE)
                                        stack.push_front( cPixel - s.width);
                        }
                        
                        if (row + 1 <= s.height) {
                                 if ( *(sImage + cPixel + s.width) == WHITE)
                                        stack.push_front( cPixel + s.width);
                        }
                        
                        if (col - 1 >= 0) {
                                 if ( *(sImage + cPixel - 1) == WHITE)
                                        stack.push_front( cPixel - 1);
                        }
                        
                        if (col + 1 <= s.width) {
                                 if ( *(sImage + cPixel + 1) == WHITE)
                                        stack.push_front( cPixel + 1);
                        }
                 }
        }
        return pixelCount;
}

```

El resultado de esta operación sería como lo siguiente:

![image-20211204112639520](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204112639520.png)

Una vez que se obtiene solamente el contorno perteneciente a la persona, es necesario rellenar dicho contorno para en etapas siguientes identificar qué pixeles pertenecen a la persona y cuáles al fondo. Para hacer esto el método siguiente fue "inspirado"  en la técnica de Programación Dinámica donde la tabla a llenar es en realidad la imagen con contornos y las soluciones "básicas" son los pixeles en 255 (Blanco) en una posición izquierda, los pixeles en en renglón anterior (arriba) y posibles pixeles en la derecha. La función fiiloutside se encarga de hacer esta tarea asi como al mismo momento poner en 0 los pixeles que son parte del fondo en la imagen BGR:

```c++
void fillOutside(cv::Mat & colorOut, cv::Mat & edges) {
        cv::Size s = edges.size();

        RGB * out = colorOut.ptr<RGB>(0);
        uint8_t * edge = edges.ptr<uint8_t>(0);
        bool found =  false;
        uint32_t startFound = 0;
        for (uint32_t row = 0; row < s.height; row++) {
                found = false;
                startFound = 0;
                for (uint32_t col = 0; col < s.width; col++, out++) {

                        uint8_t * p = edge + (row * s.width) + col;
                        if (row == 0 || col == 0 || row == s.height || col == s.width) {
                                if (*p == 0) out->blue = out->green = out->red = WHITE;
                                continue;
                        }

                        if (!found && *p == 0) {
                                out->blue = out->green = out->red = WHITE;
                                continue;
                        }

                        if (found && *p == 0 && *(p - s.width)== 0) {
                                out->blue = out->green = out->red = WHITE;
                                found = false;
                                continue;
                        }

                        if (found && *(p - s.width)== WHITE && *p == 0) {
                                continue;
                        }

                        if (found && *(p - s.width)== WHITE && *p == WHITE) {
                                for (uint32_t i = 0; i < col - startFound; i++) *(p-i) = 255;
                                startFound = col;
                                continue;
                        }
                        
                        if (*p == 255 ) {
                                found = true;
                                startFound = col;
                                continue;
                        }

                }
        }
}
```

Este método resultaría en lo siguiente:

![image-20211204113003669](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204113003669.png)

Hasta este punto este algoritmo funciona siempre y cuando objetos en el fondo no colosionen con los contornos de la persona. Para evitar este caso se optó por obtener una máscara del fondo para posteriormente usarla como referencia de objetos existentes en el fondo.  Por medio de esto se pueden detectar "colisiones" de la persona con estos objetos y por medio de una diferencia determinar si el pixel debe de ser mostrado (parte de la persona) o se debe de eliminar (perteneciente a la máscara). La sigueinte figura muestra cómo sería la máscara obtenida

![image-20211204113531058](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204113531058.png)

Esta máscara debe de ser llenada en su interior al igual que el contorno de la persona, por lo que se usó una solución similar a la de fillOutside en fillMask.

```c++
void getMask(cv::Mat & edges, cv::Mat & person, cv::Mat & mask) {
        #define WHITE  255
        cv::Size s = edges.size();

        mask = edges.clone();
        uint8_t * pp = person.ptr<uint8_t>(0);
        uint8_t * mp = mask.ptr<uint8_t>(0);
        uint8_t * ep = edges.ptr<uint8_t>(0);


        for (uint32_t row = 0; row < s.height; row++) {
                for (uint32_t col = 0; col < s.width; col++, pp++, mp++, ep++ ) {
                        if (*pp == WHITE) {
                                *mp = 0;
                        } else {
                                *mp = *ep;
                        }
                }
        }
}
void fillMask(cv::Mat & edges) {
        cv::Size s = edges.size();

        uint8_t * edge = edges.ptr<uint8_t>(0);
        bool found =  false;
        uint32_t startFound = 0;
        for (uint32_t row = 0; row < s.height; row++) {
                found = false;
                startFound = 0;
                for (uint32_t col = 0; col < s.width; col++) {

                        uint8_t * p = edge + (row * s.width) + col;
                        if (row == 0 || col == 0 || row == s.height || col == s.width) {
                                
                                continue;
                        }

                        if (!found && *p == 0) {
                                
                                continue;
                        }

                        if (found && *p == 0 && *(p - s.width)== 0) {
                                
                                found = false;
                                continue;
                        }

                        if (found && *(p - s.width)== 255 && *p == 0) {
                                continue;
                        }

                        if (found && *(p - s.width)== 255 && *p == 255) {
                                for (uint32_t i = 0; i < col - startFound; i++) *(p-i) = 255;
                                startFound = col;
                                continue;
                        }
                        
                        if (*p == 255 ) {
                                found = true;
                                startFound = col;
                                continue;
                        }

                }
        }
}
```

Una vez obtenida la máscara se pretende remover los pixeles de ella en caso de colisión por medio de el método removeMask:

```c++
void removeMask(cv::Mat & colorOut, cv::Mat & edges, cv::Mat & mask, cv::Mat & Image) {
        #define THRESHOLD 35
        cv::Size s = edges.size();
        RGB * out = colorOut.ptr<RGB>(0);
        RGB * img = Image.ptr<RGB>(0);
        uint8_t * edge = edges.ptr<uint8_t>(0);
        uint8_t * msk = mask.ptr<uint8_t>(0);

        for (uint32_t row = 0; row < s.height; row++) {
                for (uint32_t col = 0; col < s.width; col++, out++, img++, msk++, edge++ ) {

                        if (*edge == 255 && *msk == 255) {
                                if (bgrDiff(out,img) < THRESHOLD ) {
                                        out->blue = out->green = out->red = WHITE;
                                }
                        } 
                }
        }
}
```

## Pruebas y Resultados

Solo se hicieron pruebas sobre un  formato en específico 640 x 480 píxeles.

El código se creó para ser compilado en un sistema con Linux por lo que la forma de ejecutar y probar es por medio del siguiente comando.

```bash
$./backgroundRemoval
```



### Kruskal:

En esta prueba usando el método de segmentación con un fondo uniforme se obtiene la siguiente salida. 

![image-20211204114927674](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204114927674.png)

Poniendo un objeto en el fondo se observa que por el criterio de detección de fondo, este objeto no se remueve.

![image-20211204115312779](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204115312779.png)

### Contornos (DFS & PD)

A continuación se muestra el resultado de esta implementación sin objetos en el fondo.

![image-20211204115607364](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204115607364.png)

La sigueinte  imagen muestra los resultados con objetos en el fondo. Se puede observar que a diferencia de la primera implementación el objeto en la pared es removido también. 

![image-20211204115831513](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204115831513.png)



La siguiente imagen muestra el caso cuando hay una colisión entre la máscara (objetos en el fondo) con el contorno de la persona en el video. El algoritmo trata de remover la máscara en dicha colisión, pero por diferencias entre imágenes y movimientos se alcanza a percibir parte de los objetos. 

![image-20211204115907338](C:\Users\nxa11750\AppData\Roaming\Typora\typora-user-images\image-20211204115907338.png)

## Análisis de Complejidad

Debido a que la segunda implementación fue la que mostró mejores resultados, el análisis apriori en esta sección corresponde a dicha implementación. 

Se sabe que por trabajar con imágenes  y cada imágen consta de m x n pixeles se puede pensar que la entrada puede cambiar de forma cuadrática, pero debido a que las resoluciones standard no cambian en razones lineales, la entrada en realidad son N pixeles y la salida también son N Pixeles donde N = m x n, se determina que la entrada son N pixeles y se usa esta notación en el análisis apriori. 

El algoritmo inicia con una conversión a escala de grises, esta es una simple barrida de los N pixeles. Aquí se realiza una sola asignación, 2 sumas y 3 multiplicaciones. Sea que contemos sumas, multiplicaciones, o movimientos, esta ralación es siempre lineal. Sea Ng la complejidad de la conversión a escala de grises. 
$$
O(N) = Ng
$$
De igual manera tanto un filrtro gaussiano, el operador Sobel, y la acentuación de contornos se realizan sobre un solo barrido de los N pixeles. Llamemos Nf, Ns y Nth a la complejidad del filtro, Sobel y umbral. Todos estos realizan diferente cantidad de sumas, multiplicaciones, asignaciones pero todas son lineales con respecto a N.
$$
O(N) = Ng + Nf + Ns + Nth
$$
El siguiente método a analizar es la búsqueda en profundidad. En este caso no todos los pixeles se toman en cuenta sino solo los que están en la cercanía de los ojos y forman el contorno de la persona. En este DFS cada pixel tiene 4 aristas. Llamemos P  los pixeles/nodos del grafo generado. Se requiere recorrer todos los nodos P y se puede interpretar como una lista de adyacencia, donde cada nodo tiene a lo mucho 4 aristas a lo que llamaremos E, pero como cada arista aparece dos veces en realidad es 2E. Por lo que la complejidad en este caso es:

  
$$
O(N) = Ng + Nf + Ns + Nth + P + 2E
$$
Depués la sección de llenado de los contornos "usando" PD  es un solo  barrido a la imagen de N pixeles, por lo que Np es la complejidad lineal de este método. 
$$
O(N) = Ng + Nf + Ns + Nth + P + 2E + Np
$$
Los métodos de llenar las máscara y removerla de la imagen original son también un solo barrido a la imagen de N pixeles. Sea Nfm y Nrm la complejidad lineal de llenar la máscara y removerla respectivamente.
$$
O(N,E) = Ng + Nf + Ns + Nth + P + 2E + Np + Nfm + Nrm
$$
Esto indica que la complejidad del algoritmo es lineal. La tabla siguiente muestra el tiempo de ejecución dependiendo de la resolución

| Resolución | Tiempo ms |
| ---------- | --------- |
| 640 x 480  | 33ms      |
| 960 x 540  | 60ms      |
| 1280 x 720 | 100ms     |

Para la complejidad espacial, se crearon arreglos o  matrices del mismo tamaño de N pixeles para cada etapa del algoritmo por lo que la complejidad espacial al igual que la temporal es lineal:
$$
O(N) = Ng + Nf + Ns + Nth + P + Np + Nfm + Nrm
$$
La siguiente tabla muestra un porcentake de uso en memoria con el comado top para diferentes resoluciones:

| Resolución | %Memoria |
| ---------- | -------- |
| 640 x 480  | 1.0      |
| 960 x 540  | 1.2      |
| 1280 x 720 | 1.3      |

## Conclusión

Para este mismo problema se pudieron econtrar dos diferentes  "soluciones" una mejor que otra en términos de capacidad de remover el fondo. Aún se encontraron posibles mejoras al proyecto como el posible uso de un filtro de mediana para eliminar el ruido y mejorar el caso de las colisiones de contornos. Además se pudo demostrar el uso de varios algoritmos vistos en clase como el manejo de grafos, MST y prorgramación dinámica, algoritmos que a pesar de haber sido inventados hace décadas siguen sido útiles en muchas áreas computacionales. 
