void regression_lineaire(lidarAnalize_t* data,int count, double X, double Y){
    double somme_xy = 0, somme_x = 0, somme_y = 0, somme_xx=0, a,b;
    int NB = 0, ligne = 0,  pos = 0, neg = 0, prev = 0;
    double somme_angle = 0, somme_dist = 0, nb=0, distance;
    float angle;
    int rows = 100; // Nombre de lignes
    int next_valid; //permet de trouver le prochain élément valide
    double d1,d2, deg1,deg2,deg3;
    

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
        distance = data[i].dist;

        //if (position->dist < 100) {convertAngularToAxial(data, count, position);}
        if(data[i].valid && distance < 4000){
            somme_angle += data[i].angle;
            somme_dist += distance; nb ++;
            somme_xy += data[i].x*data[i].y;
            somme_x += data[i].x;
            somme_y += data[i].y;
            somme_xx += data[i].x*data[i].x;

            next_valid = 1;
            while ((!data[i+next_valid].valid) && ((i+next_valid) <count)) {next_valid++;}
            
            double ecart = data[i].dist - data[i+next_valid].dist;
            if (ecart >=0 && prev >= 0) {pos += 1;prev = 1;}
            if (ecart <=0 && prev <= 0) {neg += 1;prev = -1;}
            if (ecart >=0 && prev < 0) {pos =1;prev = 1;}
            if (ecart <=0 && prev > 0) {neg = 1;prev = -1;}
            
            if (fabs(ecart) > 200 || (pos >= 3 && neg >=3)){   // changement d'élément de décord, séparation si écart > 20cm
            angle = data[i].angle;
                if (angle <50 || (angle > 55 && angle < 110) || (angle  > 130 && angle < 230) || (angle >240 && angle < 300)){
                    if (pos >= 3 && neg >=3) {printf("\n angle coin = %f", data[i].angle);}
                    pos = 0,neg = 0;
                    d1 = data[i].dist;
                    d2 = somme_dist/nb;
                    deg1 = data[i].angle;
                    deg2 = somme_angle/nb;
                    array[ligne]->cm = 2*distance_2_pts(d1, deg1, d2,deg2);
                    array[ligne]->moy_angle = somme_angle/nb; array[ligne]->nb = nb;
                    array[ligne]->moy_dist = somme_dist/nb; array[ligne]->i = i;
                    array[ligne]->somme_x = somme_x;
                    array[ligne]->somme_xx = somme_xx;
                    array[ligne]->somme_xy = somme_xy;
                    array[ligne]->somme_y = somme_y;
                    somme_angle = 0; somme_dist = 0; nb = 0; ligne ++;
                    somme_xy = 0; somme_x = 0; somme_y = 0; somme_xx;
                }
            }
        }
    }
    //supprime le fin du tableau
    while (array[rows-1]->moy_dist == 0){
        supprimerElement(array, rows, rows -1);}

    // suppression si nb <  5 et largeur > 20cm b
    for (int i= 0; i< rows; i++){
        
        while (array[rows -1    - i]->nb < 5 || array[rows -1 - i]->cm < 200){
            supprimerElement(array, rows, rows -1 -i); 
            if (rows -1 -i < 0){break;}
        
        }}
    // Affichage pour vérifier la valeur
        for (int l = 0; l < rows; ++l) {
            //printf("\ntest = %f",array[l]->nb*array[l]->somme_xy- array[l]->somme_x*array[l]->somme_y);


            b = (array[l]->nb*array[l]->somme_xy - array[l]->somme_x*array[l]->somme_y)/(array[l]->nb*array[l]->somme_xx - array[l]->somme_x*array[l]->somme_x);
            a = (array[l]->somme_y)/(array[l]->nb) -b*(array[l]->somme_x)/(array[l]->nb);
            printf("\n Rows = %i / Angle = %f / Dist = %f / n = %i / i = %i / mm = %f / a = %f / b = %f",l, array[l]->moy_angle,array[l]->moy_dist, array[l]->nb, array[l]->i, array[l]->cm,a,b); 
        }

    /*
    for (int i = 0; i < count; i++){
        if (data[i].valid && data[i].angle < 180 && data[i].angle > 90){
            somme_xy += data[i].x*data[i].y;
            somme_x += data[i].x;
            somme_y += data[i].y;
            somme_xx += data[i].x*data[i].x;
            NB++;
        }}
    b = (NB*somme_xy - somme_x*somme_y)/(NB*somme_xx - somme_x*somme_x);
    a = somme_y/NB -b*somme_x/NB;
    printf("\na = %f / b = %f", a,b);
    */
}

double erreur(lidarAnalize_t* data,int count, double X, double Y){
    int nb=0;
    double l = 2000, L = 3000;
    double alpha, beta ;
    double distance=0;
    alpha = atan((L-2*X)/(l-2*Y))*180/M_PI;
    beta = atan((l-2*Y)/(L-2*X))*180/M_PI;

    for (int i = 0; i < count; i++){
        if (data[i].valid && (data[i].angle < alpha || data[i].angle > 360 - alpha)){
            distance += pow(data[i].y - (l/2 -Y),2)/1000;
            nb ++;}
        if (data[i].valid && (data[i].angle < alpha + 2*beta || data[i].angle > alpha)){
            distance += pow(data[i].x - (L/2 - X),2)/1000;
            nb ++;}
        if (data[i].valid && (data[i].angle < 3*alpha + 2*beta || data[i].angle > alpha + 2*beta)){
            distance += pow(data[i].y - (l/2 +Y),2)/1000;
            nb ++;}
        if (data[i].valid && (data[i].angle < 360 - alpha || data[i].angle > 3*alpha + 2*beta)){
            distance += pow(data[i].x - (L/2 + X),2)/1000;
            nb ++;}
    }
    distance = distance /nb;
    printf("\ndistance = %f", distance);
    return distance;
}
int desc_gradient(lidarAnalize_t* data, int count, position_t position){
    double X = 0, Y=0, teta = 0,gradX, gradY, gradTeta;
    int LEN = 128, TPS = 20000, LEARNING_RATE = 1.0, LEARNING_RATE_DECAY_STEP = 50, LEARNING_RATE_DECAY_FACTOR = 4.0, EPSILON = 1;


    
    for (int i = 0; i < TPS; i++){  
        gradX = (erreur(data,count, X+EPSILON,Y)-erreur(data,count, X,Y))/EPSILON;
        gradY = (erreur(data,count, X,Y+EPSILON)-erreur(data,count, X,Y))/EPSILON;
        //printf("\n ereur = %f / erreur y = %f", (erreur(data,count, X,Y)), erreur(data,count, X+EPSILON,Y));
        X-= gradX*LEARNING_RATE;
        Y-= gradY*LEARNING_RATE;
    }
    
    printf("\n X = %f / Y = %f / distance = %f",X,Y,erreur(data,count, X,Y));
    


}

void init_position_balise(lidarAnalize_t* data, int count, position_t *position){
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
    int j =0;
    int etat = 1;
    for(int i = 0; i <1.04*count; i++){
        if (data[j].angle > 359.5) {j=0;etat = 0;} //reboucle à l'angle 0 pour pas couper les balises
        distance = data[j].dist;

        //if (position->dist < 100) {convertAngularToAxial(data, count, position);}
        if(data[j].valid && distance < 5000){
            if (etat == 1){somme_angle += data[j].angle;}
            else {somme_angle += data[j].angle + 360;}
            somme_dist += distance; nb ++;

            next_valid = 1;
            while ((!data[i+next_valid].valid) && ((i+next_valid) <count)) {next_valid++;}
            if (fabs(distance- data[j+next_valid].dist) > 200){   // changement d'élément de décord, séparation si écart > 20cm
                d1 = data[j].dist;
                d2 = somme_dist/nb;
                deg1 = data[j].angle;
                deg2 = somme_angle/nb;
                array[ligne]->cm = 2*distance_2_pts(d1, deg1, d2,deg2);
                array[ligne]->moy_angle = somme_angle/nb; array[ligne]->nb = nb;
                array[ligne]->moy_dist = somme_dist/nb; array[ligne]->i = j;
                
                somme_angle = 0; somme_dist = 0; nb = 0; ligne ++;
            }
        }
        j++;
    }
        
    
    //supprime le fin du tableau
    while (array[rows-1]->moy_dist == 0){
        supprimerElement(array, rows, rows -1);}

   
    // suppression si nb < 3
    for (int i= 0; i< rows; i++){
        while (array[rows -1    - i]->nb < 3 ){
            if (array[rows -1 - i]->moy_angle > 350 || array[rows -1 - i]->moy_angle < 10) {break;}
            supprimerElement(array, rows, rows -1 -i); 
            if (rows -1 -i < 0){break;}
        }}

    
    // suppression si cm < 4cm points
    for (int i= 0; i< rows; i++){
        while (array[rows -1 - i]->cm <40 ){
            if (array[rows -1 - i]->moy_angle > 350 || array[rows -1 - i]->moy_angle < 10) {break;}
            supprimerElement(array, rows, rows -1 -i); 
            if (rows -1 -i < 0){break;}
        }}
    
    
    // suppression si cm > 15
    for (int i= 0; i< rows; i++){
        while (array[rows -1 - i]->cm >200 ){       
            supprimerElement(array, rows, rows -1 -i); 
            if (rows -1 - i < 0) {break;}
        }}
    
    /*
    // Affichage pour vérifier la valeur
    for (int l = 0; l < rows; ++l) {
        printf("\n Rows = %i / Angle = %f / Dist = %f / n = %i / i = %i / mm = %f ",l, array[l]->moy_angle,array[l]->moy_dist, array[l]->nb, array[l]->i, array[l]->cm); 
    }
    */

    // donne poto 1 et 2
    double d_1_2 , d_2_3 , d_3_1;
    int index_poto1, index_poto2, index_poto3; //poto 1 = gauche haut, poto 2 = gauche bas, poto 3 = droite
    float poto_1_2, poto_2_3, poto_3_1, d_tot = 10000;
    poto_1_2 = 1900.0;
    poto_2_3 = 3340.0; 
    poto_3_1 = 3340.0; 
    if (rows < 3){printf("\n PAS ASSEZ DE BALISE");return;} 
    for (int i= 0; i< rows; i++){
        for (int j= 0; j< rows; j++){
            for (int k=0; k<rows; k++){
                if (i!=j && i!=k && j!= k){ 
                    d_1_2 = distance_2_pts(array[i]->moy_dist, array[i]->moy_angle, array[j]->moy_dist, array[j]->moy_angle);
                    d_2_3 = distance_2_pts(array[j]->moy_dist, array[j]->moy_angle, array[k]->moy_dist, array[k]->moy_angle);
                    d_3_1 = distance_2_pts(array[k]->moy_dist, array[k]->moy_angle, array[i]->moy_dist, array[i]->moy_angle);
                    distance = fabs(d_1_2-poto_1_2) + fabs(d_2_3-poto_2_3) + fabs(d_3_1-poto_3_1);
                    //printf("\n d_1_2 = %f / d_2_3 =  %f / d_3_1 = %f / distance = %f", d_1_2 , d_2_3 , d_3_1, distance);
                    if (distance< d_tot) {
                        d_tot = distance;
                        index_poto1 = i;
                        index_poto2 = j;
                        index_poto3 = k;
                    }
                }
            }
        }
    }
    //printf("\n distance = %f", array[index_poto2]->moy_dist);
    d_1_2 = distance_2_pts(array[index_poto1]->moy_dist, array[index_poto1]->moy_angle, array[index_poto2]->moy_dist, array[index_poto2]->moy_angle);
    d_2_3 = distance_2_pts(array[index_poto2]->moy_dist, array[index_poto2]->moy_angle, array[index_poto3]->moy_dist, array[index_poto3]->moy_angle);
    d_3_1 = distance_2_pts(array[index_poto3]->moy_dist, array[index_poto3]->moy_angle, array[index_poto1]->moy_dist, array[index_poto1]->moy_angle);
    distance = fabs(d_1_2-poto_1_2) + fabs(d_2_3-poto_2_3) + fabs(d_3_1-poto_3_1);
    //printf("\n d_1_2 = %f / d_2_3 =  %f / d_3_1 = %f / distance = %f", d_1_2 , d_2_3 , d_3_1, distance);
    // vérification bon arrangement poteaux
    deg1 = array[index_poto1]->moy_angle;
    deg2 = array[index_poto2]->moy_angle;
    deg3 = array[index_poto3]->moy_angle;
    if ((deg1 < deg3 && deg3< deg2) || (deg3 < deg2 && deg2 < deg1)|| (deg2<deg1&& deg1<deg3));
    else {
        int temp = index_poto1;
        index_poto1 = index_poto2;
        index_poto2 = temp;
    }


    //determination centre 3 cercles
    //printf("\nD_2_M = %f / poto 1 = %i / poto 2 = %i / poto 3 = %i / d_1_2 = %f / d_2_3 = %f / d_3_1 = %f / distance = %f", array[index_poto2]->moy_dist, index_poto1, index_poto2, index_poto3, d_1_2, d_2_3, d_3_1, distance);
    sol_eq_2cercle(0,d_1_2,array[index_poto1]->moy_dist, 0,0, array[index_poto2]->moy_dist, sqrt(d_2_3*d_2_3 - d_1_2*d_1_2/4), d_1_2/2, array[index_poto3]->moy_dist, &position->x,&position->y);
    position->dist = array[index_poto2]->moy_dist;
    printf("\n distance = %f", distance);
    position->teta = 270 - atan(position->y/position->x)*180/M_PI - array[index_poto2]->moy_angle;
    if (position->teta < 0){ position->teta += 360;}
    //printf("\nxM = %f / yM = %f\n",position->x,position->y );
    
    
    // Libération de la mémoire
    for (int i = 0; i < rows; ++i) {
        delete array[i];}
    delete[] array;
}