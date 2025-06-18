from dearpygui import dearpygui as dpg
from screeninfo import get_monitors
import math

# get screensize 
def get_user_screen_dim():
    monitors=get_monitors()
    monitor= monitors[0]
    screen_width=monitor.width
    screen_height = monitor.height
    return((screen_width,screen_height))


# Constants
RADIUS = 125
KNOB_RADIUS = 45
WINDOW_WIDTH, WINDOW_HEIGHT= get_user_screen_dim()
Joy_stick_CENTER_X = WINDOW_WIDTH*0.75
Joy_stick_CENTER_Y = WINDOW_HEIGHT/2
joystick_pos = (Joy_stick_CENTER_X,Joy_stick_CENTER_Y)



# State
joystick_pos = [Joy_stick_CENTER_X, Joy_stick_CENTER_Y]   # initial state of joystick
normalized_output = [0.0, 0.0]       # array to store output of joystick 
is_dragging = False                  # falg to check if mouse is draging the joystick 




def update_joystick():                # main function used t update joystick on screen. 
    
    if dpg.does_item_exist("joystick knob"):  # delete prevoius instance of joystick on screen
        dpg.delete_item("joystick knob")
    else:
        dpg.draw_circle((joystick_pos[0],joystick_pos[1]), KNOB_RADIUS, color=(0, 200, 255, 255),  fill=(0, 200, 255, 100), parent="joystick_canvas",tag="joystick knob" ) # Draw joystick knob

    if (dpg.does_item_exist("joystick circle")==False):  # only draw once as it is static 
        dpg.draw_circle((Joy_stick_CENTER_X, Joy_stick_CENTER_Y), RADIUS, color=(150, 150, 150, 255), thickness=2, parent="joystick_canvas",tag="joystick circle") # Draw joystick outer circle

    if (dpg.does_item_exist("throttle outline")==False):  # only draw once as it is static 
        dpg.draw_rectangle(pmin=(400, 400), pmax=(600, 800), color=(150, 150, 150, 255), parent="joystick_canvas", tag="throttle outline")

    if dpg.does_item_exist("throttle knob"):  # delete prevoius instance of joystick on screen
        dpg.delete_item("throttle knob")
    else:
         dpg.draw_rectangle(pmin=(400, 500), pmax=(600, 575), color=(0, 200, 255, 255), parent="joystick_canvas", tag="throttle knob")

def is_point_in_circle(point, center, radius):   # function that is used to ensure the dragged knob stays within the joystick circle
    dx = center[0] - point[0]
    dy = center[1] - point[1] 
    return dx*dx + dy*dy <= radius*radius

def handle_mouse_events():
    global is_dragging, joystick_pos, normalized_output
   
    # Check mouse state every frame
    if dpg.is_mouse_button_down(dpg.mvMouseButton_Left):
        mouse_pos = dpg.get_mouse_pos()

        if not is_dragging:

            # Check if we're clicking on the knob
            if is_point_in_circle(mouse_pos, joystick_pos, KNOB_RADIUS*4):   # were are multplying the knob_radius by 4 so that user does not need to click on knob to register click. 
                is_dragging = True
        else:
            # Calculate vector from center to mouse

            dx = mouse_pos[0] - Joy_stick_CENTER_X
            dy = mouse_pos[1] - Joy_stick_CENTER_Y
            distance = math.sqrt(dx*dx + dy*dy)

            # Constrain to joystick radius using vector clamping method 
            if distance > RADIUS:
                dx = dx * RADIUS / distance
                dy = dy * RADIUS / distance
           
            # Update positions
            joystick_pos = [Joy_stick_CENTER_X + dx, Joy_stick_CENTER_Y + dy]
            normalized_output = [round(dx / RADIUS, 2), round(-dy / RADIUS, 2)]
            
            # Update display
            dpg.set_value("X_OUT", f"X: {normalized_output[0]:.2f}")
            dpg.set_value("Y_OUT", f"Y: {normalized_output[1]:.2f}")
    else:
        if is_dragging:
            is_dragging = False
            joystick_pos = [Joy_stick_CENTER_X, Joy_stick_CENTER_Y]
            normalized_output = [0.0, 0.0]
            dpg.set_value("X_OUT", "X: 0.00")
            dpg.set_value("Y_OUT", "Y: 0.00")
             
    update_joystick()

# Initialize
dpg.create_context()
dpg.create_viewport(title="Virtual Joystick", width=WINDOW_WIDTH, height=WINDOW_HEIGHT,x_pos=0,y_pos=0,decorated=False)

# Main Window
with dpg.window(tag="Primary Window", width=WINDOW_WIDTH, height=WINDOW_HEIGHT,no_resize=True,no_move=True):


    dpg.add_button(label="CLOSE",width=100,height=20,callback=lambda:dpg.stop_dearpygui(),pos=(WINDOW_WIDTH-120,10))

    with dpg.group(pos=(200,100)):
        dpg.add_text("Joystick Output:")
        dpg.add_text("X: 0.00", tag="X_OUT")
        dpg.add_text("Y: 0.00", tag="Y_OUT")
    with dpg.group(pos=(400,100)):
        dpg.add_button(label="Start", callback=lambda: print("Start Pressed"))
        dpg.add_button(label="Stop", callback=lambda: print("Stop Pressed"))
        dpg.add_button(label="Reset", callback=lambda: print("Reset Pressed"))
    
    # Drawing canvas
    with dpg.group(pos=(0,0)):
        #with dpg.drawlist(width=WINDOW_WIDTH, height=WINDOW_HEIGHT-100, tag="telemetery_canvas"):
          #  dpg.draw_rectangle(pmin=(50, 50), pmax=(200 + 500, 300 + 500),color=(255, 255, 255, 255),thickness=2,parent="telemetery_canvas" )
        with dpg.drawlist(WINDOW_WIDTH, WINDOW_HEIGHT-100, tag="joystick_canvas"):
            pass
       
 

# Final setup
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.set_primary_window("Primary Window", True)
update_joystick()
# Main loop
while dpg.is_dearpygui_running():
    
    handle_mouse_events()
    dpg.render_dearpygui_frame()
dpg.destroy_context()