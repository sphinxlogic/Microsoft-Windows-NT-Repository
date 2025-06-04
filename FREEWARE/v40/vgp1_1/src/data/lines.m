clear
t = [0:0.05:1]';
degrad = pi/180;
n = length(t);
l = 0;
line = zeros(n,3);
export = zeros(n,3);

circle(:,1) = cos(2*pi*t);
circle(:,2) = sin(2*pi*t);

for lat = -90:10:90
  line(:,1:2) = cos(lat*degrad)*circle;
  line(:,3)   = sin(lat*degrad)*ones(n,1);
  export(l+1:l+n,:) = line;
  export(l+n+1,:) = zeros(1,3);
  l = l + n + 1;
end

v1 = zeros(n,3);
v1(:,[1 3]) = circle;

for long = -90:10:90
  ang = long*degrad;
  rot = [[cos(ang) sin(ang)  0]
         [-sin(ang) cos(ang) 0]
         [0 0 1]];
  line =v1 * rot;
  export(l+1:l+n,:) = line;
  export(l+n+1,:) = zeros(1,3);
  l = l + n + 1;
end