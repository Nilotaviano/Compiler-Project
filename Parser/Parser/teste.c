int main() {
    int i, in;
    float f, fl;
    char c, ch;
    i = 10 + 20 + 30 + 40 + 50;
    in = i;
    in = in + in * in;
    f = 10;
    f = f + 1.5;
    fl = f * f;
    i = 1 * 2;
    //i = f; //ERRO: Tipo invalido
    //i = 1/2; //Erro: Tipo invalido
    
    {
        int i;
        {
            i = 10;
            //i = 1.5; //Erro: Tipo invalido
            {
                char i;
                //float i; //ERRO: variável já declarada no mesmo escopo
                i = 'i';
                //i = 1; //ERRO: tipo invalido (char i)
                {{{float i; i = 1; i = i + 2.5 + 100000;}}}
            }
        }
    }
	{
	{{{{{{{{{{i = 10;fl=i;}}}}}}}}}}
		i = 10;
	}
    {
        int f;
        {
            f = 10;
            //f = 1.5; //Erro: Tipo invalido
            {
                char f;
                //float f; //ERRO: variável já declarada no mesmo escopo
                f = 'f';
                //f = 1; //ERRO: tipo invalido (char f)
                {{{float f; f = 1; f = f + 2.5 + 100000;}}}
            }
        }
    }
    {
        int c;
        {
            c = 10;
            //c = 1.5; //Erro: Tipo invalido
            {
                char c;
                //float c; //ERRO: variável já declarada no mesmo escopo
                c = 'c';
                //c = 1; //ERRO: tipo invalido (char c)
                {{{float c; c = 1; c = c + 2.5 + 100000;}}}
            }
        }
    }
    
    if(i > f) {
        //int k, k; //Erro: Variavel já declarada no mesmo escopo
        int i, f;
        c = 'a';
        //ch = c + 1.5; //Erro: Tipo incompativel
        ch = c + 'k' + 'k' + 'k'; //Não é pra dar erro
    }
    else {
        char hue, br;
        hue = 'Z' + 'U' + 'E' + 'I' + 'R' + 'A';
        br = hue * ('N' + 'E' + 'V' + 'E' + 'R') / ('E' + 'N' + 'D' + 'S');
    }
    
    while((10 / 40) == (f * f / (fl + 2))){
        //a = a + a;  //ERRO: Variavel não declarada
        do {
            int a;
            a = 1 * 1 + 1 - 1;
        } while(1 != 1);
    }
    //i = c //ERRO: Falta ';'. *******PS: Descobri um problema no meu aqui, porque o erro apontado foi de tipo incompativel e não da falta de ';'.
}