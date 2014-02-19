
struct vector
{
    float x,y,z;
};

vector Vector(float x, float y , float z);
vector Vector(float x, float y);
vector operator+(vector &v1, vector &v2);
vector operator+(vector &v1, vector v2);
vector operator-(vector &v1, vector &v2);
//vector operator-(vector &v1, vector v2);
vector operator*(vector &v, float s);
float operator*(vector &v1, vector &v2);
//float operator*(vector &v1, vector v2);
vector operator/(vector &v, float s);
vector &operator+=(vector &v1, vector &v2);
vector &operator*=(vector &v1, float s);
