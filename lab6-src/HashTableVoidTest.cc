
#include <stdio.h>
#include "HashTableVoid.h"


struct Student {
  const char * name;
  long grade;
};

Student students[] = {
  {"Rachael", 8 },
  {"Monica", 9},
  {"Phoebe", 10},
  {"Joey", 6},
  {"Ross", 8},
  {"Chandler", 7}
};

struct Vars {
  const char * varName;
  const char * value;
};

Vars vars[] = {
  {"a", "abcd"},
  {"b", "efgh"},
  {"c", "defg"}
};

void test1()
{
  HashTableVoid h;

  bool e;
  e = h.insertItem("Rachael", (void*) 8);
  assert(!e);
  
  e = h.insertItem("Monica", (void*) 9);
  assert(!e);
  
  e = h.insertItem("Monica", (void *) 10);
  assert(e);

  printf("Test1 passed\n");
}

void test2()
{
  HashTableVoid h;

  bool e;
  e = h.insertItem("Rachael", (void*)8);
  assert(!e);
  
  e = h.insertItem("Monica", (void*)9);
  assert(!e);
  
  e = h.insertItem("Monica", (void*)10);
  assert(e);

  void * gradev;
  int grade;
  e = h.find("Rachael",&gradev);
  grade = (long)gradev;
  assert(e);
  assert(grade==8);

  e = h.find("Monica",&gradev);
  grade = (long)gradev;
  assert(e);
  assert(grade==10);

  printf("Test2 passed\n");
}

void test3()
{
  HashTableVoid h;

  for (int i=0; i<sizeof(students)/sizeof(Student);i++) {
    bool e;
    e = h.insertItem(students[i].name, (void*) students[i].grade);
    assert(!e);
  }

  for (int i=0; i<sizeof(students)/sizeof(Student);i++) {
    bool e;
    int grade;
    void * gradev;
    e = h.find(students[i].name, &gradev);
    grade = (long)gradev;
    assert(e);
    assert(grade==students[i].grade);
  }

  int grade;
  void * gradev;
  bool e = h.find("John",&gradev);
  grade = (long)gradev;
  assert(!e);

  e = h.removeElement("John");
  assert(!e);

  e = h.removeElement("Rachael");
  assert(e);

  e = h.find("Rachael",&gradev);
  grade = (long)gradev;
  assert(!e);

  printf("Test3 passed\n");
}

void test4()
{
  HashTableVoid h;

  bool e;
  e = h.insertItem("Rachael", (void*) 8);
  assert(!e);
  
  e = h.insertItem("Monica", (void*) 9);
  assert(!e);
  
  e = h.insertItem("Monica", (void*) 10);
  assert(e);

  HashTableVoidIterator iterator(&h);

  int sum=0;
  const char * key;
  int grade;
  void * gradev;
  while (iterator.next(key, gradev)) {
    grade = (long)gradev;
    sum+=grade;
    //printf("%s %d\n", key, grade);
  }

  assert(sum==18);

  printf("Test4 passed\n");
}

void test5()
{
  HashTableVoid h;

  int sum = 0;
  for (int i=0; i<sizeof(students)/sizeof(Student);i++) {
    bool e;
    e = h.insertItem(students[i].name, (void*) students[i].grade);
    assert(!e);
    sum+=students[i].grade;
  }

  HashTableVoidIterator iterator(&h);

  int sum2=0;
  const char * key;
  int grade;
  void * gradev;
  while (iterator.next(key, gradev)) {
    grade = (long)gradev;
    sum2+=grade;
    //printf("%s %d\n", key, grade);
  }

  assert(sum2==sum);
  printf("Test5 passed\n");
}

void test6()
{
  HashTableVoid h;

  for (int i=0; i<sizeof(vars)/sizeof(Vars);i++) {
    bool e;
    e = h.insertItem(vars[i].varName, (void *) vars[i].value);
    assert(!e);
  }

  for (int i=0; i<sizeof(vars)/sizeof(Vars);i++) {
    bool e;
    const char * value;
    void * valuev;
    e = h.find(vars[i].varName, &valuev);
    value = (const char *)valuev;
    assert(e);
    assert(!strcmp(value,vars[i].value));
  }

  const char *value;
  void * valuev;
  bool e = h.find("John",&valuev);
    value = (const char *)valuev;
  assert(!e);

  e = h.removeElement("John");
  assert(!e);

  e = h.removeElement("a");
  assert(e);

  e = h.find("a",&valuev);
  value = (const char *)valuev;
  assert(!e);

  printf("Test6 passed\n");

}

void
usage()
{
  // Print usage
  fprintf(stderr, "HashTableTemplateTest test1|test2|test3|test4|test5|test6\n");
}

int
main( int argc, char **argv)
{
  if (argc == 1) {
    usage();
    exit(1);
  }

  if ( !strcmp(argv[1], "test1")) {
    test1();
  }
  else if ( !strcmp(argv[1], "test2")) {
    test2();
  }
  else if ( !strcmp(argv[1], "test3")) {
    test3();
  }
  else if ( !strcmp(argv[1], "test4")) {
    test4();
  }
  else if ( !strcmp(argv[1], "test5")) {
    test5();
  }
  else if ( !strcmp(argv[1], "test6")) {
    test6();
  }
  else {
    usage();
    exit(1);
  }

  exit(0);
  
}
