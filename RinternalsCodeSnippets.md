# Introduction #

## Evaluating a C string in R ##
```
char mystring[] = "sample(10)";
SEXP parsed = eval(lang4(install("parse"),mkString("") , mkString(""),mkString(mystring)),  R_GlobalEnv);
PrintValue(eval(lang2(install("eval"),parsed), R_GlobalEnv));
```


## Converting a SEXP object to C string ##
Not sure if this is the best way to do it. Before you pass the object to C you need to deparse it (calling same R deparse function from C using evals gives different results).
`.Call("mycfunc", deparse(data))`

```
SEXP mycfunc(SEXP data){
    printf("type %s\n", CHAR(STRING_ELT(data,0)));
}
```

This is in fact a better way, but it will expand the object full which might be good or bad depending what you want to do with it.
```

void SEXP2string(SEXP Sobject, char ** strobj){
  SEXP dSobject;
  int i, size=0;
  PROTECT(dSobject = eval(lang3(install("deparse"),Sobject,ScalarInteger(500)),R_GlobalEnv));

  int lobject = LENGTH(dSobject); // how many rows in the deparsed object

  // first find out how big the object is
  for(i=0;i<lobject; i++){
    size += strlen(CHAR(STRING_ELT(dSobject,i)));
  }
  // malloc the memory then fill it up
  *strobj = calloc(size+1, sizeof(char));

  for(i=0;i<lobject; i++){
    strcat(*strobj, CHAR(STRING_ELT(dSobject,i)));
  }

  UNPROTECT(1);
}

```




## Creating variable length expression ##
```
      SEXP s, t;
         PROTECT(t = s = allocList(3));
         SET_TYPEOF(s, LANGSXP);
         SETCAR(t, install("print")); t = CDR(t);
         SETCAR(t,  CAR(a)); t = CDR(t);
         SETCAR(t, ScalarInteger(digits));
         SET_TAG(t, install("digits"));
         eval(s, env);
         UNPROTECT(1);
```