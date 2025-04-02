int main()
{
    int a;
    a=4;
    {
        int a;
        a=2;
        {
            a=3;
        }
    }

    return a;
}