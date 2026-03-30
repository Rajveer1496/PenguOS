int pow(int number,int power){ //positive powers only
    int temp = number;
    if(power == 0) return 1;

    if(power == 1) return number;

    for(int i=1;i<power;i++){
        number *= temp;
    }

    return number;
}