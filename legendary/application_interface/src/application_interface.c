#include <kore/kore.h>
#include <kore/http.h>

int view_restful_get(struct http_request *req);
int view_restful_put(struct http_request *req);
int view_restful(struct http_request *req);

int
view_restful_get(struct http_request *req)
{
    /* object_read lib */
	http_response(req, 200, NULL, 0);
	return (KORE_RESULT_OK);
}

int
view_restful_put(struct http_request *req)
{
    /* object_write lib */
	http_response(req, 200, NULL, 0);
	return (KORE_RESULT_OK);
}

int
view_restful(struct http_request *req)
{
    /* GET/PUT router */

    switch (req->method) {

    case HTTP_METHOD_GET:
        return view_restful_get(req);
        break;

    case HTTP_METHOD_PUT:
        return view_restful_put(req);
        break;

    default:
        /* error */
	    return (KORE_RESULT_OK);
    }
}
