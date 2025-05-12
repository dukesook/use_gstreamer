#include <gst/gst.h>

void run_pipeline_example() {
  // Hello World
  // GstElement *pipeline = gst_parse_launch("videotestsrc ! autovideosink", nullptr);

  // H.264 (AVC)
  // GstElement *pipeline = gst_parse_launch("videotestsrc num-buffers=150 ! videoconvert ! openh264enc ! h264parse ! mp4mux ! filesink location=output.mp4", nullptr);

  // H.265 (HEVC) - ERROR! x265enc is not found
  // GstElement *pipeline = gst_parse_launch("videotestsrc num-buffers=150 ! videoconvert ! x265enc ! h265parse ! mp4mux ! filesink location=output_h265.mp4", nullptr);

  // H.265 (HEVC) - nvh265enc
  // GstElement *pipeline = gst_parse_launch("videotestsrc num-buffers=150 ! videoconvert ! nvh265enc ! h265parse ! mp4mux ! filesink location=output.mp4", nullptr);

  // Rust Plugin - SUCCESS
  // isomp4mux
  // GstElement *pipeline = gst_parse_launch("videotestsrc num-buffers=150 ! videoconvert ! nvh265enc ! h265parse ! isomp4mux ! filesink location=output.mp4", nullptr);

  // Uncompressed
  GstElement *pipeline = gst_parse_launch("videotestsrc num-buffers=34 ! video/x-raw,format=I420,width=640,height=480 ! isomp4mux ! filesink location=unc.mp4", nullptr);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  GstBus *bus = gst_element_get_bus(pipeline);
  GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                               (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  if (msg)
    gst_message_unref(msg);

  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
}

int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);

  // Create elements
  GstElement *pipeline = gst_pipeline_new("uncompressed-pipeline");
  GstElement *src = gst_element_factory_make("videotestsrc", "src");
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
  GstElement *mux = gst_element_factory_make("isomp4mux", "mux");
  GstElement *sink = gst_element_factory_make("filesink", "sink");

  if (!pipeline || !src || !capsfilter || !mux || !sink) {
    g_printerr("Failed to create one or more elements.\n");
    return -1;
  }

  // Set element properties
  g_object_set(src, "num-buffers", 34, NULL);
  g_object_set(sink, "location", "out/unc_programatically.mp4", NULL);

  // Set caps (format=I420, width=640, height=480)
  GstCaps *caps = gst_caps_new_simple(
      "video/x-raw",
      "format", G_TYPE_STRING, "I420",
      "width", G_TYPE_INT, 640,
      "height", G_TYPE_INT, 480,
      NULL);
  g_object_set(capsfilter, "caps", caps, NULL);
  gst_caps_unref(caps);

  // Build the pipeline
  gst_bin_add_many(GST_BIN(pipeline), src, capsfilter, mux, sink, NULL);
  if (!gst_element_link_many(src, capsfilter, mux, sink, NULL)) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  // Run the pipeline
  gst_element_set_state(pipeline, GST_STATE_PLAYING);
  GstBus *bus = gst_element_get_bus(pipeline);
  gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                             (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  // Cleanup
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}
