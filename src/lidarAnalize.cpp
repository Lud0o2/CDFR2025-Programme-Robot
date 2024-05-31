#include "../Include/lidarAnalize.h"
#include <math.h>

void convertAngularToAxial(lidarAnalize_t* data, int count, position_t *position){
    for(int i = 0; i< count; i++){
        if(data[i].valid){
            data[i].x = data[i].dist*cos((data[i].angle+ position->teta)*DEG_TO_RAD) + position->x;
            data[i].y = -data[i].dist*sin((data[i].angle+position->teta)*DEG_TO_RAD) + position->y;
            /*
            //get table valid
            if(data[i].x<1800 && data[i].x>-100 && data[i].y<2900 && data[i].y>-50)
                data[i].onTable = true;
            else
                data[i].onTable = false;
            */
        }
    }
    
}

void printLidarAxial(lidarAnalize_t* data, int count){
    for(int i = 0; i< count; i++){
        const char* charMessage = "          ";
        if(!data[i].valid){
            charMessage = "non Valide";
        }
        if(data[i].onTable){
            charMessage = "non Table ";
        }
        printf("%s teta : %lf \t x : %d \ty : %d\n",charMessage,data[i].angle,data[i].x,data[i].y);
    }
}

void printAngular(lidarAnalize_t* data, int count){
    for(int i = 0; i< count; i++){
        if(data[i].valid){
            printf("theta: %03.2f \tDist: %08.2f\n",data[i].angle,data[i].dist);
        }
        else{
            printf("non valid\n");
        }
    }
}

void maxDistance(lidarAnalize_t* data, int count,int& maxX, int maxY){
    maxX = 0;
    maxY = 0;
    for(int i = 0; i< count; i++){
        if(abs(data[i].x) > maxX)
            maxX = abs(data[i].x);
        if(abs(data[i].y) > maxY)
            maxY = abs(data[i].y);
    }
}

bool collideFordward(lidarAnalize_t* data, int count){
    for(int i = 0; i <count; i++){
        if(data[i].valid && data[i].onTable)
            if(data[i].angle <45 || data[i].angle>(360-45))
                if(data[i].dist < 500){
                    return true;
                }
    }
    return false;
}

bool collideBackward(lidarAnalize_t* data, int count){
    for(int i = 0; i <count; i++){
        if(data[i].valid && data[i].onTable)
            if(data[i].angle<(180+45) && data[i].angle>(180-45))
                if(data[i].dist < 500){
                    return true;
                }
    }
    return false;
}

int collide(lidarAnalize_t* data, int count ,int distanceStop){
    static int PreviousDistanceStop  = 0;

    //keep direction
    if(distanceStop!=0){
        if(distanceStop >= 0){
            PreviousDistanceStop = 1;
        }
        else{
            PreviousDistanceStop = -1 ;
        }
    }
    else{
        distanceStop = PreviousDistanceStop;
    }

    int iRet = 12000; //maximum capation distance for lidar
    for(int i = 0; i <count; i++){
        if(data[i].valid && data[i].onTable){    
            if(distanceStop >= 0){
                if(data[i].angle <45 || data[i].angle>(360-45)){
                    if(data[i].dist-distanceStop < iRet){
                        iRet = data[i].dist-distanceStop;
                    }
                }
            }
            else{
                if(data[i].angle<(180+45) && data[i].angle>(180-45)){
                    if(data[i].dist+distanceStop < iRet){
                        iRet = data[i].dist+distanceStop;
                    }
                }
            }
        }
    }
    //LOG_DEBUG("Distance Stop : ",distanceStop);
    //LOG_DEBUG("Collide : ",iRet);
    // LOG_DEBUG("Prec : ",iRetPre);
    return iRet;
}

void pixelArtPrint(lidarAnalize_t* data, int count,int sizeX,int sizeY,int scale,position_t position){
    char* matriceAffichage;
    matriceAffichage = (char*)malloc(sizeX * sizeY * sizeof(char));
    
    //initMatrice
    for(int i = 0; i<sizeX * sizeY; i++){
        matriceAffichage[i] = ' ';
    }

    int posix, posiy;
    for(int i = 0; i<count; i++){
        if(data[i].valid == true){
            posix = data[i].x/scale + sizeX/2;
            posiy = data[i].y/scale + sizeY/2;
            if(posix<sizeX && posix>=0 && posiy<sizeY && posiy>=0){
                if(data[i].onTable)
                    matriceAffichage[posix + sizeX * posiy] = '*';
                else
                    matriceAffichage[posix + sizeX * posiy] = 'X';
            }                
            else{
                if(posix>=sizeX)
                    posix = sizeX-1;
                if(posix<0)
                    posix = 0;
                if(posiy>=sizeY)
                    posiy = sizeY-1;
                if(posiy<0)
                    posiy = 0;
                matriceAffichage[posix + sizeX * posiy] = 'W';
            }
        }
    }
    //fill

    int positionRoboty = position.x/scale + sizeX/2;
    int positionRobotx = position.y/scale + sizeY/2;

    for(int i = 0; i<sizeX; i++){
        char chartype = ' ';
        for(int j = positionRoboty; j<sizeY; j++){
            int posX = MAP(j,positionRoboty,sizeY,positionRobotx,i);
            if(matriceAffichage[posX + sizeX * j] != ' '){
                chartype = matriceAffichage[posX + sizeX * j];
            }                
            if(chartype != ' '){
                matriceAffichage[posX + sizeX * j] = chartype;
            }
        }
    }

    for(int i = 0; i<sizeX; i++){
        char chartype = ' ';
        for(int j = positionRoboty; j>=0; j--){
            int posX = MAP(j,positionRoboty,0,positionRobotx,i);
            if(matriceAffichage[posX + sizeX * j] != ' '){
                chartype = matriceAffichage[posX + sizeX * j];
            }                
            if(chartype != ' '){
                matriceAffichage[posX + sizeX * j] = chartype;
            }
        }
    }


    for(int j = 0; j<sizeY; j++){
        char chartype = ' ';
        for(int i = positionRobotx; i<sizeX; i++){
            int posY = MAP(i,positionRobotx,sizeX,positionRoboty,j);
            if(matriceAffichage[i + sizeX * posY] != ' '){
                chartype = matriceAffichage[i + sizeX * posY];
            }                
            if(chartype != ' '){
                matriceAffichage[i + sizeX * posY] = chartype;
            }
        }
    }

    for(int j = 0; j<sizeY; j++){
        char chartype = ' ';
        for(int i = positionRobotx; i>=0; i--){
            int posY = MAP(i,positionRobotx,0,positionRoboty,j);
            if(matriceAffichage[i + sizeX * posY] != ' '){
                chartype = matriceAffichage[i + sizeX * posY];
            }                
            if(chartype != ' '){
                matriceAffichage[i + sizeX * posY] = chartype;
            }
        }
    }
    
    //add border
    int posiyPos, posiyNeg;
    for(int i = 0; i<sizeX; i+=(sizeX/scale)+1){
        posix = i;
        posiyPos = 1600/scale + sizeY/2;
        posiyNeg = (-1600)/scale + sizeY/2;
        if(posix < sizeX && posix >= 0 && posiyPos < sizeY && posiyPos >= 0 && posiyNeg < sizeY && posiyNeg >= 0){
            matriceAffichage[posix + sizeX * posiyPos] = 'Z';
            matriceAffichage[posix + sizeX * posiyNeg] = 'Z';
        }
    }
    int posixPos, posixNeg;
    for(int i = 0; i<sizeY; i+=(sizeY/scale)+1){
        posixNeg = (-1000)/scale + sizeX/2;
        posixPos = 1000/scale + sizeX/2;
        posiy = i;
        if(posixPos < sizeX && posixPos >= 0 && posixNeg < sizeX && posixNeg >= 0  && posiyNeg < sizeY && posiyNeg >= 0){
            matriceAffichage[posixPos + sizeX * posiy] = 'Z';
            matriceAffichage[posixNeg + sizeX * posiy] = 'Z';
        }
    }

    posix = position.x/scale + sizeX/2;
    posiy = position.y/scale + sizeY/2;
    if(posix<sizeX && posix>=0 && posiy<sizeY && posiy>=0)
        matriceAffichage[posix + sizeX * posiy] = 'O';

    //print
    for(int x = sizeX-1; x>=0; x--){
        for(int y = sizeY-1; y>=0; y--){
            printf("%c%c",matriceAffichage[x + y * sizeX],matriceAffichage[x + y * sizeX]);
        }
        printf("\n");
    }

    free(matriceAffichage);

}
void supprimerElement(element_decord**& array, int& rows, int index) {
    if (index < 0 || index >= rows) {
        printf("\n ERREUR INDEX SupprimerElement");
        return;} // Assurez-vous que l'index est valide

    element_decord** newArray = new element_decord*[rows - 1];
    
        // Copier les éléments (sauf celui à supprimer) dans le nouveau tableau
        int newIdx = 0;
        for (int i = 0; i < rows; ++i) {
            if (i != index) {
                newArray[newIdx++] = array[i];
            } else {
                delete array[i]; // Libérer la mémoire de l'élément à supprimer
            }
        }

        // Libérer la mémoire de l'ancien tableau
        delete[] array;

        // Réassigner le pointeur et la taille
        array = newArray;
        rows--;
        
}


void position_facile(lidarAnalize_t* data,int count, double X, double Y){
    int angle_parfait[4];
    double X1,X2,Y1,Y2,a;
    int i_max=0, angle_index;
    int l1,l2,L1,L2,l = 1985,L = 1907,m;
    float alpha1, alpha2,min,ecart;
    int offset= 0;
    for(int i = 0; i <count; i++){
        i_max ++;
    }
    angle_index = i_max/4;
    for (int num = 0;num < 4; num++){

        min = 1000;
        printf("\n");
        for (int i = 0; i < 10; i++){
            ecart = fabs(data[num*angle_index+i-5].angle - num*90);
            //printf(" %f /",data[num*angle_index+i-5].angle);
            if (ecart < min) { 
                min = ecart;
                angle_parfait[num] = num*angle_index+i-5;             
            }
            
        }
    }

    printf("\n angle : %f / %f / %f / %f \n", data[angle_parfait[0]].angle, data[angle_parfait[1]].angle, data[angle_parfait[2]].angle, data[angle_parfait[3]].angle);
    l1 = data[angle_parfait[2] + offset].dist;printf("%i / ",l1);
    l2 = data[0+offset].dist; printf("%i / ",l2);
    L1 = data[angle_parfait[3] + offset].dist; printf("%i / ",L1);
    L2 = data[angle_parfait[1] + offset].dist; printf("%i \n ",L2);

    if (l1+l2 < L1+L2){m = l1;l1 = L1;L1 = m;
        m = l2;l2 = L2;L2 = m;}

    alpha1 = acos(float(l)/(l1+l2))*180/M_PI;
    alpha2 = acos(float(L)/(L1+L2))*180/M_PI;
    printf("\n alpha 1 = %f / alpha 2 = %f / diff = %f / al1 = %f / al2 = %f",alpha1,alpha2, alpha1 - alpha2,float(l)/(l1+l2),float(L)/(L1+L2) );
    a = cos((alpha1+alpha2)/2*M_PI/180);
    Y1 = l1*a;
    Y2 = l2*a;
    X1 = L1*a;
    X2 = L2*a;
    printf("\n x1 = %f /x2 = %f / y1 = %f / y2 = %f / diff = %f / %f",X1,X2,Y1,Y2, l - fabs(Y1 + Y2), L- fabs(X1 + X2) );

    }

double distance_2_pts(double d1,double deg1, double d2, double deg2){
    double alpha;
    double d3;
    alpha = fabs((deg1-deg2)*M_PI/180);
    d3 = sqrt(pow(d1,2) + pow(d2,2) -2*d1*d2*cos(alpha));
    return d3;
}

void init_position_sol(lidarAnalize_t* data, int count, position_t *position){
    double distance;
    double d1,d2, deg1,deg2,deg3;
    int rows = 100; // Nombre de lignes
    int next_valid; //permet de trouver le prochain élément valide
    int ligne =0;
    double somme_angle = 0, somme_dist = 0, nb=0;
    

    
    // Allocation dynamique du tableau
    element_decord** array = new element_decord*[rows];
    for (int i = 0; i < rows; ++i) {
        array[i] = new element_decord; // Allocation de mémoire pour chaque élément
        array[i]->moy_angle = 0; // Initialisation de chaque élément
        array[i]->nb = 0;array[i]->moy_dist = 0;
        array[i]->nb = 0; array[i]->i = 0;  
    }

    //fragmente le décord en plusieurs éléments proches
    
    for(int i = 0; i <count; i++){
        //reboucle à l'angle 0 pour pas couper les balises
        distance = data[i].dist;

        //if (position->dist < 100) {convertAngularToAxial(data, count, position);}
        if(data[i].valid && distance < 4000){
            somme_dist += distance; nb ++;
            somme_angle += data[i].angle;

            next_valid = 1;

            while ((!data[i+next_valid].valid) && ((i+next_valid) <count)) {next_valid++;}
            printf("\n distance = %f / angle = %f", data[i].dist - data[i + next_valid].dist, data[i].angle);
            if (fabs(distance- data[i+next_valid].dist) > 50){   // changement d'élément de décord, séparation si écart > 20cm
                d1 = data[i].dist;
                d2 = somme_dist/nb;
                deg1 = data[i].angle;
                deg2 = somme_angle/nb;
                array[ligne]->cm = 2*distance_2_pts(d1, deg1, d2,deg2);
                array[ligne]->moy_angle = somme_angle/nb; array[ligne]->nb = nb;
                array[ligne]->moy_dist = somme_dist/nb; array[ligne]->i = i;
                somme_angle = 0; somme_dist = 0; nb = 0; ligne ++;
            }
        }
    }

    //supprime le fin du tableau
    while (array[rows-1]->moy_dist == 0){
        supprimerElement(array, rows, rows -1);}
    
    // suppression si nb <  5 et largeur < 20cm b
    for (int i= 0; i< rows; i++){
        
        while (array[rows -1    - i]->nb < 5 || array[rows -1 - i]->cm <200){
            supprimerElement(array, rows, rows -1 -i); 
            if (rows -1 -i < 0){break;}
        
        }}


    // Affichage pour vérifier la valeur
    for (int l = 0; l < rows; ++l) {
        printf("\n Rows = %i / Angle = %f / Dist = %f / n = %i / i = %i / mm = %f ",l, array[l]->moy_angle,array[l]->moy_dist, array[l]->nb, array[l]->i, array[l]->cm); 
    }
}

