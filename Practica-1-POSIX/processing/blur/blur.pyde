def setup():
    size(1024,512)
    global img
    img = loadImage("cat.jpg")
    blur = createImage(img.width, img.height, RGB)
    #loadPixels()
    
    w = img.width
    h = img.height
    kS = 15
    k = (kS - 1)/2 
    at = lambda x,y: x + y * w

    #Calcular el reflejo del indice dentro de los limites. 
    def mirror(a, N):
      if a < 0:
          return -a - 1
      elif a > N - 1:
          return N + N - 1 - a    
      else: 
        return a           
    
    for cx in range(img.width):
        for cy in range(img.height):
            R = 0
            G = 0
            B = 0
                                
            for j in range(-k,k + 1):
                sR = 0
                sG = 0
                sB = 0
                ny = mirror(cy + j, h)
                for i in range(-k,k + 1):
                    nx = mirror(cx + i,w)
                    sR+=red(img.pixels[at(nx,ny)])
                    sG+=green(img.pixels[at(nx,ny)])
                    sB+=blue(img.pixels[at(nx,ny)])  
                R+= sR/kS
                G+= sG/kS
                B+= sB/kS
            R/=kS
            G/=kS
            B/=kS                        
            blur.pixels[at(cx,cy)] = color(R,G,B)
                           
        
    #updatePixels()
    image(blur, 0, 0, width/2, height);
    image(img, width/2, 0, width/2, height);
    
