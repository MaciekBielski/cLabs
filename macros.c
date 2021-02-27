//1.
#define VARFOO(fmt, ...) do{ fprintf(stderr, fmt, __VA_ARGS__); }while(0)
#define VARFOO2(fmt, ...) do{ fprintf(stderr, fmt, ##__VA_ARGS__); }while(0)

//2. Avoiding swallowed semicolon - two null expressions next to each other
//   This is doable in two ways.
#define OUTF(a) ({ fprintf(stdout,"%f\n", (a) ); })
#define OUTS(a) do{ fprintf(stdout,"%s\n", (a) ); }while(0)

//3. Avoiding unsafe macros with GNU extension
/* no space between name and arguments in DEFINITION */
#define MIN(X, Y) ({ \
    __typeof__(X) _x=(X); \
    __typeof__(Y) _y=(Y); \
    (_x<_y) ? _x : _y; \
    })
//static int give5(){ puts("--give5--"); return 5;}
//static int give3(){ puts("--give3--"); return 5;}

//4.
#define STRINGIFY(X) ({ printf("%s\n", (#X)); })

//5.
// Nested macro
#define SQUARE( arg ) (arg*arg)
#define SQUARE2( arg ) ({ arg*arg })

//6. Stringification example
#define PARSE_SZ 4
#define FMT_PATT(width) fun_##width##(float blah)
#define FMT(arg) FMT_PATT(arg)

#define TEST(a, b, c, d)  \
	struct { \
		a b; \
		c d; \
	}

#define WRITER_OPTIONAL(writer, obj, field, writer_type_name) ({    \
    if ((obj).field)) {                                            \
        (writer).String(#field);                                    \
        (writer).writer_type_name((obj).field);                 \
    }})


int main(int argc, char** argv)
{
    WRITER_OPTIONAL(writer, data.subdata, position, Uint);
    //1.
    // VARFOO("INFO: %s\n", "bar");
    // VARFOO("INFO: empty");

    // VARFOO2("INFO: %s\n", "bar");
    // VARFOO2("INFO: empty");

    //2.
    //  if(argc==1)
    //    OUTF(3.14);
    //  else
    //    OUTF(2.74);
    //  if(argc==1)
    //    OUTS("argc==1");
    //  else
    //    OUTS("argc!=1");

    //3.
     // printf("number is: %d\n", MIN(give5(),give3()) );


    //4.
    //  STRINGIFY(MIN(4.5,8.3));

    //5. CUBE
    //SQUARE( SQUARE(5));

    //6.
    // FMT_PATT(foo);
    // FMT(PARSE_SZ);
}



