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
throttle_pos= [[350,725],[650,775]]

Rectangle_min_max=[[400, 400],[600, 750]]
# State

normalized_joystick_output = [0.0, 0.0]       # array to store output of joystick 
normalized_throttle_output = 0.0      # array to store output of throttle
knob_is_dragging = False                  # falg to check if mouse is draging the joystick 
throttle_is_dragging = False



def update_joystick():                # main function used t update joystick on screen. 
    
    if dpg.does_item_exist("joystick knob"):  # delete prevoius instance of joystick on screen
        dpg.delete_item("joystick knob")
    else:
        dpg.draw_circle((joystick_pos[0],joystick_pos[1]), KNOB_RADIUS, color=(0, 200, 255, 255),  fill=(0, 200, 255, 100), parent="joystick_canvas",tag="joystick knob" ) # Draw joystick knob

    if (dpg.does_item_exist("joystick circle")==False):  # only draw once as it is static 
        dpg.draw_circle((Joy_stick_CENTER_X, Joy_stick_CENTER_Y), RADIUS, color=(150, 150, 150, 255), thickness=2, parent="joystick_canvas",tag="joystick circle") # Draw joystick outer circle

def update_throttle():
    if (dpg.does_item_exist("throttle outline")==False):  # only draw once as it is static 
        dpg.draw_rectangle(pmin=Rectangle_min_max[0], pmax=Rectangle_min_max[1], color=(150, 150, 150, 255), parent="joystick_canvas", tag="throttle outline")

    if dpg.does_item_exist("throttle knob"):  # delete prevoius instance of joystick on screen
        dpg.delete_item("throttle knob")
    else:
         dpg.draw_rectangle(pmin=throttle_pos[0], pmax=throttle_pos[1], color=(0, 200, 255, 255),fill=(200,200,200,200), parent="joystick_canvas", tag="throttle knob")

def is_point_in_circle(point, center, radius):   # function checks if user has clicked on knob area and returns true or flase. 
    dx = center[0] - point[0]
    dy = center[1] - point[1] 
    return dx*dx + dy*dy <= radius*radius

def is_point_in_throttle_rectangle(point,min,max): # function checks if user is clicking inside the throttle area and return true or flase. 
    if point[0]>=(min[0]-50) and point[0]<=(max[0]+50) and point[1]>=min[1] and point[1]<=max[1]: 
        return True
    else:
        return False


def handle_mouse_events():
    global knob_is_dragging, joystick_pos, normalized_joystick_output,throttle_is_dragging,normalized_throttle_output
   
    # Check mouse state every frame
    if dpg.is_mouse_button_down(dpg.mvMouseButton_Left):    
        mouse_pos = dpg.get_mouse_pos()
        if not (knob_is_dragging or throttle_is_dragging):
            
            if is_point_in_throttle_rectangle(mouse_pos,Rectangle_min_max[0],Rectangle_min_max[1]):  
                throttle_is_dragging = True


            # Check if we're clicking on the knob
            if is_point_in_circle(mouse_pos, joystick_pos, KNOB_RADIUS*4):   # were are multplying the knob_radius by 4 so that user does not need to click on knob to register click. 
                knob_is_dragging = True    
                
        elif(knob_is_dragging):
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
            normalized_joystick_output = [round(dx / RADIUS, 2), round(-dy / RADIUS, 2)]
            
            # Update display
            dpg.set_value("X_OUT", f"X: {normalized_joystick_output[0]:.2f}")
            dpg.set_value("Y_OUT", f"Y: {normalized_joystick_output[1]:.2f}")


        elif(throttle_is_dragging):
            
            dy_1=mouse_pos[1]
            dy_2=mouse_pos[1]+50

            # calmping throttle 
            if dy_1<375:
                dy_1=375
                dy_2=425
            if dy_2>775:
                dy_1=725
                dy_2=775    

            #updating positions
            throttle_pos[0][1]=dy_1
            throttle_pos[1][1]=dy_2

            #converting power as a 0-100 percentage
            normalized_throttle_output = (1-(abs(((dy_1+dy_2)/2)-400) / (750-400)))*100

            #update display
            dpg.set_value("Y_OUT_Throttle", f"Power: {int(normalized_throttle_output)} %")


            print(normalized_throttle_output)
            print(throttle_pos)
    else:
        if knob_is_dragging:
            knob_is_dragging = False
            joystick_pos = [Joy_stick_CENTER_X, Joy_stick_CENTER_Y]
            normalized_joystick_output = [0.0, 0.0]
            dpg.set_value("X_OUT", "X: 0.00")
            dpg.set_value("Y_OUT", "Y: 0.00")

        if throttle_is_dragging:
            throttle_is_dragging = False     
            
    update_joystick()
    update_throttle()

def handle_key_events():
    
    if dpg.is_key_pressed(dpg.mvKey_W):
        print('w pressed')

        if(throttle_pos[0][1]>375):
            throttle_pos[0][1]-=10
            throttle_pos[1][1]-=10
    
  
    if dpg.is_key_pressed(dpg.mvKey_S):
        print('s pressed')

        if(throttle_pos[1][1]<775):
            throttle_pos[0][1]+=10
            throttle_pos[1][1]+=10

    if dpg.is_key_pressed(dpg.mvKey_1):
        throttle_pos[0][1]=725
        throttle_pos[1][1]=775

    if dpg.is_key_pressed(dpg.mvKey_2):
        throttle_pos[0][1]=637.5
        throttle_pos[1][1]=687.5    

    if dpg.is_key_pressed(dpg.mvKey_3):
        throttle_pos[0][1]=550
        throttle_pos[1][1]=600
   
    if dpg.is_key_pressed(dpg.mvKey_4):
        throttle_pos[0][1]=462
        throttle_pos[1][1]=512
       

    if dpg.is_key_pressed(dpg.mvKey_5):
        throttle_pos[0][1]=375
        throttle_pos[1][1]=425 
    
    #converting power as a 0-100 percentage
    normalized_throttle_output = (1-(abs(((throttle_pos[0][1]+throttle_pos[1][1])/2)-400) / (750-400)))*100  
       

        #update display
    dpg.set_value("Y_OUT_Throttle", f"Power: {int(normalized_throttle_output)} %")

# Initialize
dpg.create_context()
dpg.create_viewport(title="Virtual Joystick", width=WINDOW_WIDTH, height=WINDOW_HEIGHT,x_pos=0,y_pos=0,decorated=False)

# Main Window
with dpg.window(tag="Primary Window", width=WINDOW_WIDTH, height=WINDOW_HEIGHT,no_resize=True,no_move=True):


    dpg.add_button(label="CLOSE",width=100,height=20,callback=lambda:dpg.stop_dearpygui(),pos=(WINDOW_WIDTH-120,10))

    with dpg.group(pos=(1400,100)):
        dpg.add_text("Joystick Output:")
        dpg.add_text("X: 0.00", tag="X_OUT")
        dpg.add_text("Y: 0.00", tag="Y_OUT")
    with dpg.group(pos=(400,100)):
        dpg.add_text("Throttle Output:")
        dpg.add_text("Power : 0 %", tag="Y_OUT_Throttle")
  
    
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
update_throttle()
# Main loop
while dpg.is_dearpygui_running():
    
    handle_mouse_events()
    handle_key_events()
    dpg.render_dearpygui_frame()
dpg.destroy_context()