static double Square(double x) {
    double Result = (x * x);
    return Result;
}

static double RadiansFromDegrees(double degrees) {
    double Result = (degrees * M_PI / 180.0);
    return Result;
}

static double ReferenceHaversine(double x0, double y0, double x1, double y1, double EarthRadius) {
    double lat1 = y0;
    double lat2 = y1;
    double lon1 = x0;
    double lon2 = x1;

    double dLat = RadiansFromDegrees(lat2 - lat1);
    double dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);

    double a = Square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*Square(sin(dLon/2));
    double c = 2.0*asin(sqrt(a));
    
    double Result = EarthRadius * c;
    
    return Result;
}
