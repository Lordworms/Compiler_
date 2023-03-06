void main() {
    int64 n
    n<- input()

    int64[] arr
    arr <- new Array(n)

    int64 i
    i<-0
    int64 f
    f<-1
    int64 x
    int64 y
    int64 k
:beg_init
    x<-input()
    arr[i]<-x
    i<-i+1
    if (i=n):end_init :beg_init
:end_init
    
    int64 j
    j<-0
    i<-0
    int64 len
    len<-n-1
:inner
    k<-j+1
    x<-arr[j]
    y<-arr[k]
    if(x>y):swap_beg :swap_end
    :swap_end
    j<-j+1
    if(j<len):inner :outer
:outer
    j<-0
    len<-len-1
    i<-i+1
    if(i<n):inner :end
:swap_beg
    arr[j]<-y
    arr[k]<-x
    if(f=1):swap_end :end
:end
    print(arr)
}