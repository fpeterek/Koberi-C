/* Libraries */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

/* Typedefs */

typedef double flt_type;
typedef intmax_t int_type;
typedef uintmax_t uint_type;
typedef unsigned char uchar;

/* Memory allocator */

void* _mem_allocator(int_type size, void* (*fun)(void*)) {
    void * object = malloc(size);
    if (fun) {
        fun(object);
    }
    return object;
}

/* Object copy function - copies objects and preservers vtable ptrs */

void _copy_object(void * dest, void * orig, size_t objectSize) {

    dest += sizeof(void*);
    orig += sizeof(void*);

    objectSize -= sizeof(void*);

    memcpy(dest, orig, objectSize);

}



/* Classes */

typedef struct Object
{
    void (**vtable)(void);
} Object;

typedef struct Shape
{
    void (**vtable)(void);
} Shape;

typedef struct Square
{
    void (**vtable)(void);
    int_type side;
} Square;

typedef struct Circle
{
    void (**vtable)(void);
    int_type radius;
} Circle;



/* Global Variables */




/* Function Declarations */

void Object_m__koberic__vtable_init(Object* self);
int_type _koberic_fact_int(int_type x);
void Shape_m__koberic_destruct(Shape* self);
int_type Shape_m__koberic_area(Shape* self);
void Shape_m__koberic__vtable_init(Shape* self);
int_type Square_m__koberic_area(Square* self);
void Square_m__koberic_destruct(Square* self);
void Square_m__koberic__vtable_init(Square* self);
int_type Circle_m__koberic_area(Circle* self);
void Circle_m__koberic_destruct(Circle* self);
void Circle_m__koberic__vtable_init(Circle* self);
int_type _koberic_main();
void _koberic__globalVarInit();



/* Virtual method tables */

void (*Object_vt__vtable[])(void) = {
};

void (*Shape_vt__vtable[])(void) = { 
    (void (*)(void))Shape_m__koberic_destruct,
    (void (*)(void))Shape_m__koberic_area
};

void (*Square_vt__vtable[])(void) = { 
    (void (*)(void))Square_m__koberic_destruct,
    (void (*)(void))Square_m__koberic_area
};

void (*Circle_vt__vtable[])(void) = { 
    (void (*)(void))Circle_m__koberic_destruct,
    (void (*)(void))Circle_m__koberic_area
};



/* Function Definitionss */

void Object_m__koberic__vtable_init(Object* self)
{
    self->vtable = Object_vt__vtable;
}


int_type _koberic_fact_int(int_type x)
{
    if (0ll == x)
    {
        
        {
            return( 1ll );
        }

    }

    
    {
        return( (x * _koberic_fact_int((x - 1ll))) );
    }

}


void Shape_m__koberic_destruct(Shape* self)
{
    fputs("Shape destructor", stdout);
    fputs("\n", stdout);
}


int_type Shape_m__koberic_area(Shape* self)
{
    
    {
        return( 0ll );
    }

}


void Shape_m__koberic__vtable_init(Shape* self)
{
    self->vtable = Shape_vt__vtable;
}


int_type Square_m__koberic_area(Square* self)
{
    
    {
        return( (self->side * self->side) );
    }

}


void Square_m__koberic_destruct(Square* self)
{
    fputs("Square destructor \n", stdout);
}


void Square_m__koberic__vtable_init(Square* self)
{
    self->vtable = Square_vt__vtable;
}


int_type Circle_m__koberic_area(Circle* self)
{
    
    {
        return( (self->radius * self->radius * 3.14159) );
    }

}


void Circle_m__koberic_destruct(Circle* self)
{
    fputs("Circle destructor \n", stdout);
}


void Circle_m__koberic__vtable_init(Circle* self)
{
    self->vtable = Circle_vt__vtable;
}


int_type _koberic_main()
{
    Circle circle;
    Circle_m__koberic__vtable_init((&circle));
    circle.radius = 5ll;
    fputs("Circle with radius ", stdout);
    printf("%lld", circle.radius);
    fputs(" has area ", stdout);
    printf("%lld", Circle_m__koberic_area((&circle)));
    fputs("\n", stdout);
    Shape* shape = (&circle);
    fputs("Shape area: ", stdout);
    printf("%lld", ((int (*)(Shape*))shape->vtable[1])(shape));
    fputs("\n", stdout);
    fputs("5! = ", stdout);
    printf("%lld", _koberic_fact_int(5ll));
    fputs("\n", stdout);
    Circle_m__koberic_destruct((&circle));
}


void _koberic__globalVarInit()
{
}



/* C Main Function */

int main(int argc, const char * argv[]) {
    _koberic__globalVarInit();
    return _koberic_main();
}
