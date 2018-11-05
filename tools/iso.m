## Copyright (C) 2018 iko
## 
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*- 
## @deftypefn {Function File} {@var{retval} =} isometric_transformation (@var{input1}, @var{input2})
##
## @seealso{}
## @end deftypefn

## Author: iko <iko@Brutzelkiste>
## Created: 2018-03-24

function iso(file_name_in, ang)

warning('off', 'all');

PIC_in = imread(file_name_in);
PIC_in = flipud(PIC_in');

[w, h] = size(PIC_in);

b = transform(w, 0, ang);
xr = ceil(b(1));
b = transform(0, h, ang);
xl = floor(b(1));
w_out = xr - xl;
b = transform(w, h, ang);
h_out = round(b(2));
shift_x = -xl + 1;

PIC_out = zeros(w_out, h_out);

for x_in = 1:1:w
  for y_in = 1:1:h
    
    PIC_in(x_in, y_in);
    
    b = transform(x_in, y_in, ang);
    
    x_out = round(b(1)) + shift_x;
    y_out = round(b(2));
    
    if PIC_in(x_in, y_in) > 0
      PIC_out(x_out, y_out) = PIC_in(x_in, y_in);
    end
    
  end
end

PIC_out = fliplr(PIC_out');

% write file:
file_name_out = [file_name_in(1:end-4), '_', num2str(ang), '.bmp'];
imwrite(PIC_out, file_name_out);

endfunction


function b = transform(x, z, ang)

% constants:
alpha = asin(tan(pi * 60 / 360));
beta = pi * ang / 180;

% input vector:
a = [x; 0; z];

% rotation around horizontal x axis:
A = [ ...
  [1,     0,     0]; ...
  [0,  cos(alpha), sin(alpha)]; ...
  [0, -sin(alpha), cos(alpha)]];

% rotation around vertical y axis:
B = [ ...
  [cos(beta), 0, -sin(beta)]; ...
  [        0, 1,          0]; ...
  [sin(beta), 0,  cos(beta)]];
  

b = A * B * a;

endfunction
