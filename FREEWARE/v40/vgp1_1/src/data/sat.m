clear
t = -0.12:0.01:4;

f1(1,:) = cos(2*pi*t);
f1(2,:) = sin(2*pi*t);
f1(3,:) = zeros(size(t));

inc = -65*pi/180;   % 65 Degrees.

rot = [[1 0 0]
       [0 cos(inc) sin(inc)]
       [0 -sin(inc) cos(inc)]];

f2 = f1' *rot;

for i = 1:length(t)
  ang = -t(i) * (2*pi/15) + 4.35;
  rot = [[cos(ang) sin(ang)  0]
         [-sin(ang) cos(ang) 0]
         [0 0 1]];
  f3(i,:) = f2(i,:) *rot;
end

save -ascii orbit.asc f3
