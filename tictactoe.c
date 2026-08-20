char*l,b[]="123\n456\n789";p=79;main(c,d){puts(b);for(l="!A";d=*l++;)if((b[c=d/8]&b[c+d%8]&b[c+d%8*2])==p)return;for(;c=getchar()-49,c<0|b[d=c+c/3]-c-49;);main(b[d]=p^=23);}
