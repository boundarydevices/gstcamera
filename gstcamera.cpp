#include "gst/gst.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <linux/videodev2.h>
#include "v4l2-extra.h"
#include <sys/ioctl.h>

static void trimCtrl(char *buf){
        char *tail = buf+strlen(buf);
        // trim trailing <CR> if needed
        while ( tail > buf ) {
                --tail ;
                if ( iscntrl(*tail) ) {
                        *tail = '\0' ;
                }
                else
                        break;
        }
}

static int camera_fd = 5; /* from /proc/`pidof gstcamera`/fd */

static void process_command(char *cmd)
{
	if (0 == strcasecmp("af",cmd)) {
		struct v4l2_control c;
		c.id=V4L2_CID_AUTO_FOCUS_START;
		int ret=ioctl(camera_fd,VIDIOC_S_CTRL,(v4l2_control *)&c);
		if(ret==-1)
			printf("\n ioctl V4L2_CID_AUTO_FOCUS_START fail: %m\n ");
		else
			printf("\nioctl V4L2_CID_AUTO_FOCUS_START sucessful: %d\n");
	} else if (0 == strncasecmp("sp",cmd,2)) {
		struct v4l2_send_command_control vc;
		vc.id = 105;
		vc.value0 = strtoul(cmd+2,0,0);
		int ret = ioctl(camera_fd,VIDIOC_SEND_COMMAND,(v4l2_send_command_control*)&vc);
		if(ret==-1)
			printf("\n ioctl STEP fail: %m\n ");
		else
			printf("\nioctl STEP sucessful: %d\n",ret);
	} else if (0 == strncasecmp("fd",cmd,2)) {
		camera_fd = strtoul(cmd+2,0,0);
		printf("camera_fd set to %d\n", camera_fd);
	} else
                printf("unknown command %s\n", cmd);
}

int main (int argc, char const *const argv[])
{
	printf("Hello %s\n", argv[0]);
	gst_init(NULL, NULL);
	char pipeline_string[512] = {0};
	for (int arg=1; arg < argc ; arg++) {
		strcat(pipeline_string, argv[arg]);
		strcat(pipeline_string, " ");
	}

	pipeline_string[strlen(pipeline_string)-1] = 0;
	printf("Pipeline string is: %s\n", pipeline_string);

	// Create pipeline from configuration string
	GstElement *pipeline = gst_parse_launch(pipeline_string, NULL);
	printf( "pipeline == %p\n", pipeline);

	// Retrieve pipeline signal bus
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	printf( "bus == %p\n", bus);

	// Set pipeline to ready
	gst_element_set_state(pipeline, GST_STATE_READY);

	printf("pipeline is ready\n");
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	char inbuf[80];
	while (0 != fgets(inbuf,sizeof(inbuf),stdin)) {
		trimCtrl(inbuf);
		process_command(inbuf);
	}
	return 0 ;
}

#if 0
/**
 * Create instance of GstPlayer for playing a video
 * @param play_unaccelerated	true if you should play with acceleration, false otherwise
 */
GstPlayer::GstPlayer()
{
	mode = UNACCELERATED;
	init_pipeline();
}

/**
 * Destroy instance of GstPlayer
 */
GstPlayer::~GstPlayer()
{
}

/**
 * Play video
 */
void
GstPlayer::play()
{
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

/**
 * Pause video
 */
void
GstPlayer::pause()
{
	gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

/**
 * Stop video
 */
void
GstPlayer::stop()
{
	destroy();
	init_pipeline();
}

/**
 * Destroy this instance of GstPlayer by removing all references to components.
 * Necessary step in some cases before starting the stream after a STOP, so
 * this should be called whenever restarting the stream
 */
void
GstPlayer::destroy()
{
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	gst_object_unref(bus);
}

/**
 * Provide reference to pipeline bus for monitoring pipe status
 * @return	reference to pipeline bus
 */
GstBus*
GstPlayer::get_bus()
{
	return bus;
}


/**
 * Initialize pipeline based on configuration file
 */
void
GstPlayer::init_pipeline()
{
	const int MAX_LENGTH = 500;
	char pipeline_string[MAX_LENGTH];

	// Retrieve pipeline configuration
	if (mode == ACCELERATED)
		get_config("pipeline_fast", pipeline_string);
	else if (mode == UNACCELERATED)
		get_config("pipeline", pipeline_string);
	else
		get_config("pipeline_camera", pipeline_string);

	printf("Pipeline string is: %s\n", pipeline_string);

	// Create pipeline from configuration string
	pipeline = gst_parse_launch(pipeline_string, NULL);

	// Retrieve pipeline signal bus
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));

	// Set pipeline to ready
	gst_element_set_state(pipeline, GST_STATE_READY);

}

/**
 * Set whether this video should play with acceleration or not
 * By default, this resets the pipeline, so do not call this midstream.
 * @param newMode	new mode of play, whether accelerated, nonaccelerated,
 * 	camera, or other
 */
void
GstPlayer::set_mode(unsigned int newMode)
{
	mode = newMode;
	stop();
}
#endif
