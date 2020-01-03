#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"

class triangle: public hitable {
    public:
        triangle() {}
        //sphere(vec3 cen, float r, material *m): center(cen), radius(r), mat_ptr(m) {};
        triangle(vec3 p1,vec3 p2, vec3 p3, material *m): pt1(p1), pt2(p2), pt3(p3), mat_ptr(m) {};
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        //points
        vec3 pt1;
        vec3 pt2;
        vec3 pt3;
        material *mat_ptr;
};

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    /*
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0){
        float  temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
    */
    
    
    
    //TODO: implement triangle-ray intersection
    
    const float EPSILON = 0.0000001;
    vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = pt2 - pt1;
    edge2 = pt3 - pt1;
    
    h = cross(r.direction(),edge2);
    //h = rayVector.crossProduct(edge2);
    
    a = dot(edge1,h);
    //a = edge1.dotProduct(h);
    
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = r.origin() - pt1;
    
    u = f*dot(s,h);
    //u = f*s.dotProduct(h);
    if (u < 0.0 || u > 1.0)
        return false;
        
    q = cross(s, edge1);    
    //q = s.crossProduct(edge1);
    
    v = f*dot(r.direction(),q);    
    //v = f * rayVector.dotProduct(q);
    
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    
    float t = f*dot(edge2, q);
    //float t = f * edge2.dotProduct(q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
            rec.t = t;
            rec.p = r.point_at_parameter(rec.t);
            if (dot(cross(edge1,edge2),r.direction())>0){
                rec.normal = -1*unit_vector(cross(edge1,edge2));
            }else{
                rec.normal = unit_vector(cross(edge1,edge2));
            }
            //rec.normal = unit_vector(cross(edge1,edge2));
            //rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        //outIntersectionPoint = rayOrigin + rayVector * t;
        //return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;

}

#endif