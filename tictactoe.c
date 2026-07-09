char*l,b[]="123\n456\n789";p=79;main(c,d){puts(b);for(l="1Ia4:@5?";*l;)if((b[d=(c=*l++-48)/6]&b[d+c%6]&b[d+c%6*2])==p)return;do c=getchar()-49;while(c<0|b[c+c/3]-c-49);b[c+c/3]=p^=23;main();}
