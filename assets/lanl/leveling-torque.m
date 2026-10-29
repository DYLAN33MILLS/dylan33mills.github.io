clc
clear

% Parameters:
weight_lbs = 13;         % Weight of the panel in pounds
width_inches = 18;       % Width of the panel in inches
height_inches = 16;      % Height of the panel in inches
angle_deg = 90;          % Desired rotation angle in degrees
time_seconds = 10;       % Desired time to achieve the rotation angle
motor_offset = 0;      % Offset of motor from the panel's center of mass in inches

torque_required = calculate_torque(weight_lbs, width_inches, height_inches, angle_deg, time_seconds, motor_offset);

function torque_required = calculate_torque(weight_lbs, width_inches, height_inches, angle_deg, time_seconds, motor_offset_inches)
    
    % Constants
    g = 32.2;  
    lbs_to_slugs = 1 / g;  
    inches_to_feet = 1 / 12;  
    deg_to_rad = pi / 180;  
    ft_lbs_to_nm = 1.35582;  
    inch_to_meter = 0.0254;

    % Convert inputs
    mass_slugs = weight_lbs * lbs_to_slugs;  
    width_feet = width_inches * inches_to_feet;  
    height_feet = height_inches * inches_to_feet;  
    angle_rad = angle_deg * deg_to_rad;  
    
    % angular acceleration (alpha)
    alpha = (2 * angle_rad) / (time_seconds ^ 2);  % rad/s^2
    
    % moment of inertia (I)
    I = (1 / 12) * mass_slugs * (width_feet^2 + height_feet^2);
    
    % rotational torque
    torque_rotational_ftlbs = I * alpha;  % Torque in ft-lbs
    torque_rotational = torque_rotational_ftlbs * ft_lbs_to_nm;  % Convert to Nm
    
    % additional torque due to offset
    mass_kg = weight_lbs * 0.453592; 
    offset_meters = motor_offset_inches * inch_to_meter;  
    torque_offset = mass_kg * 9.81 * offset_meters;  
    
    % Total required torque
    torque_required = torque_rotational + torque_offset;
    
    % Display results
    fprintf('Rotational Torque: %.4f Nm\n', torque_rotational);
    fprintf('Offset Torque: %.4f Nm\n', torque_offset);
    fprintf('Total Required Torque: %.4f Nm\n', torque_required);
end
