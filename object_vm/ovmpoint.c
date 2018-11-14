#include "ovm.c"

// Object subclass: #Point instanceVariableNames: 'x y' "create a new class Point(x, y) by subclassing from Object"
struct mypoint { _VTABLE_REF; struct object *x; struct object *y; };
typedef struct mypoint *HPoint; 

static struct vtable *Point_vt;
static struct object *Point;

static struct symbol *xpyp;
static struct symbol *get_x;
static struct symbol *get_y;
static struct symbol *offset;

static struct object *Point_newp(struct closure *cls, struct object *self,int x,int y)
{
	HPoint clone = (HPoint)send(vtof(self), s_vtallocate, sizeof(struct mypoint));

	clone->x = i2oop(x);
	clone->y = i2oop(y);
	return (struct object *)clone;
}

static struct object *Point_x(struct closure *cls, struct mypoint *self)
{

	return self->x;
}

static struct object *Point_y(struct closure *cls, struct mypoint *self)
{
	return self->y;
}

static struct object *Point_offset(struct closure *cls, struct mypoint *receiver, struct mypoint *delta)
{
	HPoint offset_point = (HPoint)send(vtof(receiver), s_vtallocate, sizeof(struct mypoint));
	
	int offset_x = oop2i(send(receiver,get_x))+oop2i(send(delta,get_x));
	int offset_y = oop2i(send(receiver,get_y))+oop2i(send(delta,get_y));

	offset_point->x = i2oop(offset_x);
	offset_point->y = i2oop(offset_y);
	return (struct object *)offset_point;
}

static struct object *Point_basicSize(struct closure *cls, struct object *self)
{
	assert(self);
	assert(szof(self) < 4096); // catch bad pointers
	return i2oop(szof(self));
}

int main(int argc,char *argv[])
{
	init_ovm();

	xpyp = (typeof(xpyp))send(Symbol,s_newp,"x:y:");
	get_x = (typeof(get_x))send(Symbol,s_newp,"x:");
	get_y = (typeof(get_y))send(Symbol,s_newp,"y:");
	offset = (typeof(offset))send(Symbol,s_newp,"offset:");
	s_basicSize = (typeof(s_basicSize)) send(Symbol, s_newp, "basicSize");

	Point_vt = (typeof(Point_vt))send(vtof(Object),s_vtdelegate,"Point");
	Point = (typeof(Point))send((struct object *)Point_vt,s_vtallocate,0);

	dump_vt(Proto_vt);
	dump_vt(Object_vt);
	dump_vt(Symbol_vt);
	dump_obj(Symbol);
	dump_vt(Point_vt);
	dump_obj(Point);

	assert(vtof(Point) == Point_vt);

	send(Point_vt, s_vtadd_method, xpyp, (method_t)Point_newp);
	send(Point_vt, s_vtadd_method, get_x, (method_t)Point_x);
	send(Point_vt, s_vtadd_method, get_y, (method_t)Point_y);
	send(Point_vt, s_vtadd_method, offset, (method_t)Point_offset);
	send(Point_vt, s_vtadd_method, s_basicSize, (method_t)Point_basicSize);

	struct object *start = send(Point, xpyp, 10, 20);
	struct object *delta = send(Point, xpyp, 5, 8);

	struct object *end = send(start, offset, delta);
	struct object *size_start = send(start, s_basicSize);
	struct object *proto_vt = send(Proto_vt, s_basicSize);

	dump_vt(Point_vt);
	dump_obj(start);
	dump_obj(delta);
	dump_obj(end);

	printf("start: %d@%d\n",oop2i(send(start,get_x)),oop2i(send(start,get_y)) );
	printf("delta: %d@%d\n",oop2i(send(delta,get_x)),oop2i(send(delta,get_y)) );
	printf("end  : %d@%d\n",oop2i(send(end,get_x)),oop2i(send(end,get_y)) );

	printf("Size of start: %d\n", oop2i(send(start,s_basicSize)));
	
}